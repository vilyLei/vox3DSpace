// Visual Test: Fast Hot-Reload
//
// Purpose: Verify that the hot-reload system correctly detects script file changes
//          and recompiles without crashing.
//
// Behavior:
//   - 1 rectangle starts red
//   - Every 3 seconds the test automatically overwrites color.glsl with a new color
//   - Cycle: Red -> Green -> Blue -> Yellow -> Magenta -> Cyan -> Red ...
//   - scene_.reloadScripts() is called every frame
//   - Console prints each reload event with timestamp
//
// Pass criteria:
//   - Rectangle color changes to match the written color at each 3s interval
//   - No crash or freeze during color transitions
//   - Console shows "[Reload] t=X.X -> <color>" messages
//   - Runs stably for 20+ seconds (at least 6 color changes)

#define GLFW_INCLUDE_VULKAN
#include <filesystem>
#include <fstream>
#include <iomanip>
#include "application/platform/window.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_pipeline.hpp"
#include "rhi/vulkan/vulkan_uniform_buffer.hpp"
#include "renderer/renderer.hpp"
#include "scene/scene.hpp"
#include "scene/render_object.hpp"
#include "core/types.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <memory>
#include <string>

using namespace vkapp;

static const std::vector<Vertex> rectVertices = {
    {{-0.3f, -0.3f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.3f, -0.3f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}},
    {{-0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> rectIndices = {0, 1, 2, 2, 3, 0};

// Color sequence: 6 entries, cycle every 3 seconds
struct ColorEntry {
    const char* name;
    const char* glsl; // inline vec4 literal
};

static const ColorEntry COLOR_CYCLE[] = {
    {"Red",     "vec4(1.0, 0.2, 0.2, 1.0)"},
    {"Green",   "vec4(0.2, 1.0, 0.2, 1.0)"},
    {"Blue",    "vec4(0.2, 0.2, 1.0, 1.0)"},
    {"Yellow",  "vec4(1.0, 1.0, 0.2, 1.0)"},
    {"Magenta", "vec4(1.0, 0.2, 1.0, 1.0)"},
    {"Cyan",    "vec4(0.2, 1.0, 1.0, 1.0)"},
};
static constexpr int   NUM_COLORS     = 6;
static constexpr float RELOAD_INTERVAL = 3.0f; // seconds per color

class FastReloadTest {
public:
    void run() {
        init();
        mainLoop();
        cleanup();
        std::cout << "Test completed successfully." << std::endl;
    }

private:
    static constexpr int WIDTH  = 600;
    static constexpr int HEIGHT = 600;

    std::unique_ptr<Window>          window_;
    vk::Instance                     instance_;
    vk::SurfaceKHR                   surface_;
    std::unique_ptr<VulkanDevice>    device_;
    std::unique_ptr<VulkanSwapChain> swapChain_;
    std::unique_ptr<VulkanPipeline>  pipeline_;
    std::unique_ptr<Renderer>        renderer_;
    vk::RenderPass                   renderPass_;
    vk::Semaphore                    imageAvailable_;
    vk::Semaphore                    renderFinished_;
    vk::Fence                        inFlightFence_;
    bool                             framebufferResized_ = false;

    Scene       scene_;
    std::string colorScriptPath_; // path to the writable color.glsl
    int         lastColorIndex_ = -1;

    void init() {
        std::cout << "[DEBUG] Current working directory: "
                  << std::filesystem::current_path() << std::endl;

        createInstance();

        window_ = std::make_unique<Window>(WIDTH, HEIGHT, "Test: Fast Hot-Reload");
        window_->setResizeCallback([this](int, int) { framebufferResized_ = true; });
        surface_ = window_->createSurface(instance_);

        device_    = std::make_unique<VulkanDevice>(instance_, surface_);
        swapChain_ = std::make_unique<VulkanSwapChain>(device_.get(), surface_, WIDTH, HEIGHT);

        renderPass_ = createRenderPass();
        swapChain_->createFramebuffers(renderPass_);

        VulkanUniformBuffer tempUbo(device_.get());
        pipeline_ = std::make_unique<VulkanPipeline>(
            device_.get(), renderPass_,
            "shaders/rect.vert.spv", "shaders/rect.frag.spv",
            swapChain_->getExtent(),
            tempUbo.getDescriptorSetLayout()
        );

        renderer_ = std::make_unique<Renderer>(device_.get(), swapChain_.get(), pipeline_.get());

        // Auto-detect script directory
        std::string scriptDir;
        if (std::filesystem::exists("Release/scripts/test_vis_fast_reload")) {
            scriptDir = "Release/scripts/test_vis_fast_reload/";
            std::cout << "[DEBUG] Using Release/scripts/" << std::endl;
        } else if (std::filesystem::exists("Debug/scripts/test_vis_fast_reload")) {
            scriptDir = "Debug/scripts/test_vis_fast_reload/";
            std::cout << "[DEBUG] Using Debug/scripts/" << std::endl;
        } else {
            scriptDir = "scripts/test_vis_fast_reload/";
            std::cout << "[DEBUG] Using scripts/ (fallback)" << std::endl;
        }

        colorScriptPath_ = scriptDir + "color.glsl";

        // Write initial color (Red)
        writeColorScript(0, 0.0f);

        RenderObjectDesc d;
        d.vertices    = rectVertices;
        d.indices     = rectIndices;
        d.scriptPaths = {colorScriptPath_,
                         scriptDir + "rotation.glsl",
                         scriptDir + "position.glsl"};
        scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));

        createSyncObjects();

        std::cout << "Initialization OK. Color cycle every " << RELOAD_INTERVAL
                  << "s: Red->Green->Blue->Yellow->Magenta->Cyan->..." << std::endl;
        std::cout << "Close window to end test." << std::endl;
    }

    // Overwrite color.glsl with a new solid color
    void writeColorScript(int colorIndex, float t) {
        const ColorEntry& c = COLOR_CYCLE[colorIndex % NUM_COLORS];
        std::ofstream f(colorScriptPath_);
        if (!f) {
            std::cerr << "[Reload] ERROR: cannot write to " << colorScriptPath_ << std::endl;
            return;
        }
        f << "vec4 colorScript(float t) {\n"
          << "    return " << c.glsl << ";\n"
          << "}\n";
        f.close();
        std::cout << "[Reload] t=" << std::fixed << std::setprecision(1) << t
                  << "s -> " << c.name << std::endl;
        lastColorIndex_ = colorIndex;
    }

    vk::RenderPass createRenderPass() {
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format         = swapChain_->getImageFormat();
        colorAttachment.samples        = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout    = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout     = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorRef;

        vk::SubpassDependency dep{};
        dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass    = 0;
        dep.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.srcAccessMask = vk::AccessFlagBits::eNone;
        dep.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        vk::RenderPassCreateInfo info{};
        info.attachmentCount = 1;
        info.pAttachments    = &colorAttachment;
        info.subpassCount    = 1;
        info.pSubpasses      = &subpass;
        info.dependencyCount = 1;
        info.pDependencies   = &dep;

        return device_->getDevice().createRenderPass(info);
    }

    void createSyncObjects() {
        vk::SemaphoreCreateInfo si{};
        imageAvailable_ = device_->getDevice().createSemaphore(si);
        renderFinished_ = device_->getDevice().createSemaphore(si);

        vk::FenceCreateInfo fi{};
        fi.flags       = vk::FenceCreateFlagBits::eSignaled;
        inFlightFence_ = device_->getDevice().createFence(fi);
    }

    void mainLoop() {
        auto startTime = std::chrono::high_resolution_clock::now();

        while (!window_->shouldClose()) {
            window_->pollEvents();
            drawFrame(startTime);
        }

        device_->getDevice().waitIdle();
    }

    void drawFrame(const std::chrono::high_resolution_clock::time_point& startTime) {
        device_->getDevice().waitForFences(inFlightFence_, VK_TRUE, UINT64_MAX);

        bool     outOfDate  = false;
        uint32_t imageIndex = swapChain_->acquireNextImage(imageAvailable_, outOfDate);

        if (outOfDate || framebufferResized_) {
            recreateSwapChain();
            return;
        }

        device_->getDevice().resetFences(inFlightFence_);

        auto  now    = std::chrono::high_resolution_clock::now();
        float t      = std::chrono::duration<float>(now - startTime).count();

        // Determine which color should be active now
        int colorIndex = static_cast<int>(t / RELOAD_INTERVAL) % NUM_COLORS;
        if (colorIndex != lastColorIndex_) {
            writeColorScript(colorIndex, t);
        }

        // Hot-reload: check if scripts changed on disk
        scene_.reloadScripts();

        float aspect = static_cast<float>(window_->getWidth())
                     / static_cast<float>(window_->getHeight());

        glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 2.f),
                                     glm::vec3(0.f),
                                     glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 proj = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);
        proj[1][1] *= -1.f;

        scene_.update(t, view, proj);

        renderer_->beginFrame(imageIndex);
        scene_.drawAll(renderer_->getCommandBuffer(), pipeline_->getLayout());
        renderer_->endFrame();
        renderer_->submit(imageAvailable_, renderFinished_, inFlightFence_);

        swapChain_->present(imageIndex, renderFinished_, outOfDate);

        if (outOfDate || framebufferResized_) {
            recreateSwapChain();
        }
    }

    void recreateSwapChain() {
        device_->getDevice().waitIdle();
        swapChain_->recreate(window_->getWidth(), window_->getHeight(), renderPass_);
        framebufferResized_ = false;
    }

    void cleanup() {
        device_->getDevice().waitIdle();

        device_->getDevice().destroyFence(inFlightFence_);
        device_->getDevice().destroySemaphore(renderFinished_);
        device_->getDevice().destroySemaphore(imageAvailable_);

        // Must clear scene (RenderObject buffers) before destroying device
        scene_.clear();

        renderer_.reset();
        device_->getDevice().destroyRenderPass(renderPass_);
        pipeline_.reset();
        swapChain_.reset();
        device_.reset();

        instance_.destroySurfaceKHR(surface_);
        instance_.destroy();

        window_.reset();
        glfwTerminate();
    }

    void createInstance() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName   = "FastReloadTest";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        glfwInit();
        uint32_t     count = 0;
        const char** exts  = glfwGetRequiredInstanceExtensions(&count);
        std::vector<const char*> extensions(exts, exts + count);

        vk::InstanceCreateInfo ci{};
        ci.pApplicationInfo        = &appInfo;
        ci.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        ci.ppEnabledExtensionNames = extensions.data();

        instance_ = vk::createInstance(ci);
    }
};

int main() {
    std::cout << "=== TEST: test_vis_fast_reload ===" << std::endl;
    std::cout << "=== Visual Test: Hot-Reload Script ===" << std::endl;
    std::cout << "Purpose: Verify hot-reload detects file changes and recompiles correctly" << std::endl;
    std::cout << std::endl;
    std::cout << "PASS criteria:" << std::endl;
    std::cout << "  - Rectangle color changes every 3 seconds automatically" << std::endl;
    std::cout << "  - Sequence: Red->Green->Blue->Yellow->Magenta->Cyan->Red..." << std::endl;
    std::cout << "  - Console shows [Reload] messages at each change" << std::endl;
    std::cout << "  - No crash during 20+ seconds (6+ color changes)" << std::endl;
    std::cout << std::endl;

    try {
        FastReloadTest test;
        test.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
