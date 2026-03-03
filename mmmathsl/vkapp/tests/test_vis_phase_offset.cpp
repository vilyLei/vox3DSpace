// Visual Test: Phase Offset
//
// Purpose: Verify that time variable 't' is correctly passed to each script,
// and multiple instances can produce phase-shifted behaviors from the same time base.
//
// Expected visual result:
//   - 3 rectangles with different colors (Red, Green, Blue)
//   - They move vertically in sinusoidal motion with 120° phase offset
//   - Creates a wave-like motion effect
//
// Pass criteria:
//   - Color: Three rectangles are Red, Green, Blue respectively
//   - Phase: Motion has sequence, forming a wave pattern
//   - Synchronization: Same period, only phase differs
//   - Independence: Each rectangle moves without interference
//
// Failure indicators:
//   - Wrong colors (scripts not executing correctly)
//   - Same phase motion (phase offset not working)
//   - Different periods (time variable passed incorrectly)
//   - Only one rectangle moves (instances not independent)

#define GLFW_INCLUDE_VULKAN
#include <filesystem>
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

using namespace vkapp;

// Rectangle mesh data
static const std::vector<Vertex> rectVertices = {
    {{-0.15f, -0.15f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.15f, -0.15f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.15f,  0.15f}, {1.0f, 1.0f, 1.0f}},
    {{-0.15f,  0.15f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> rectIndices = {0, 1, 2, 2, 3, 0};

class PhaseOffsetTest {
public:
    void run() {
        init();
        mainLoop();
        cleanup();
        std::cout << "Test completed successfully." << std::endl;
    }

private:
    static constexpr int WIDTH  = 800;
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

    Scene scene_;

    void init() {
        std::cout << "[DEBUG] Current working directory: "
                  << std::filesystem::current_path() << std::endl;

        createInstance();

        window_ = std::make_unique<Window>(WIDTH, HEIGHT, "Test: Phase Offset");
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

        // Auto-detect script directory (Release or Debug)
        std::string scriptDir;
        if (std::filesystem::exists("Release/scripts/test_vis_phase_offset")) {
            scriptDir = "Release/scripts/test_vis_phase_offset/";
            std::cout << "[DEBUG] Using Release/scripts/" << std::endl;
        } else if (std::filesystem::exists("Debug/scripts/test_vis_phase_offset")) {
            scriptDir = "Debug/scripts/test_vis_phase_offset/";
            std::cout << "[DEBUG] Using Debug/scripts/" << std::endl;
        } else {
            scriptDir = "scripts/test_vis_phase_offset/";
            std::cout << "[DEBUG] Using scripts/ (fallback)" << std::endl;
        }

        // Create Object 1 (Red, 0° phase)
        RenderObjectDesc obj1;
        obj1.vertices    = rectVertices;
        obj1.indices     = rectIndices;
        obj1.scriptPaths = {scriptDir + "obj1_color.glsl",
                            scriptDir + "obj1_rotation.glsl",
                            scriptDir + "obj1_position.glsl"};
        scene_.addObject(std::make_unique<RenderObject>(device_.get(), obj1));

        // Create Object 2 (Green, 120° phase)
        RenderObjectDesc obj2;
        obj2.vertices    = rectVertices;
        obj2.indices     = rectIndices;
        obj2.scriptPaths = {scriptDir + "obj2_color.glsl",
                            scriptDir + "obj2_rotation.glsl",
                            scriptDir + "obj2_position.glsl"};
        scene_.addObject(std::make_unique<RenderObject>(device_.get(), obj2));

        // Create Object 3 (Blue, 240° phase)
        RenderObjectDesc obj3;
        obj3.vertices    = rectVertices;
        obj3.indices     = rectIndices;
        obj3.scriptPaths = {scriptDir + "obj3_color.glsl",
                            scriptDir + "obj3_rotation.glsl",
                            scriptDir + "obj3_position.glsl"};
        scene_.addObject(std::make_unique<RenderObject>(device_.get(), obj3));

        createSyncObjects();

        std::cout << "Initialization OK." << std::endl;
        std::cout << "Expected: Three rectangles (Red, Green, Blue) with wave motion." << std::endl;
        std::cout << "Obj1 (Red):   y = 0.3 * sin(t)       [0 deg phase]" << std::endl;
        std::cout << "Obj2 (Green): y = 0.3 * sin(t+2.09)  [120 deg phase]" << std::endl;
        std::cout << "Obj3 (Blue):  y = 0.3 * sin(t+4.19)  [240 deg phase]" << std::endl;
        std::cout << "Close window to end test." << std::endl;
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
        fi.flags        = vk::FenceCreateFlagBits::eSignaled;
        inFlightFence_  = device_->getDevice().createFence(fi);
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
        appInfo.pApplicationName   = "PhaseOffsetTest";
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
    std::cout << "=== TEST: test_vis_phase_offset ===" << std::endl;
    std::cout << "=== Visual Test: Phase Offset ===" << std::endl;
    std::cout << "Purpose: Verify time variable 't' and phase-shifted behaviors" << std::endl;
    std::cout << std::endl;
    std::cout << "PASS criteria:" << std::endl;
    std::cout << "  - Color: Red, Green, Blue rectangles" << std::endl;
    std::cout << "  - Phase: Sequential motion forming wave pattern" << std::endl;
    std::cout << "  - Sync: Same period, different phase" << std::endl;
    std::cout << "  - Independence: No interference between objects" << std::endl;
    std::cout << std::endl;

    try {
        PhaseOffsetTest test;
        test.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
