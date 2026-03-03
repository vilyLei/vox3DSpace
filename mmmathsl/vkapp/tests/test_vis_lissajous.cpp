// Visual Test: Lissajous Figures
//
// Purpose: Verify sin/cos compound calculations in mmrsl scripts.
// 6 objects each trace a different Lissajous figure with different a:b frequency ratios.
//
// Objects:
//   Obj1 (Red)    1:1  - circle/ellipse
//   Obj2 (Green)  1:2  - figure-8
//   Obj3 (Blue)   1:3  - three-lobe
//   Obj4 (Yellow) 2:3  - butterfly
//   Obj5 (Magenta)3:4  - complex curve
//   Obj6 (Cyan)   3:5  - star-like
//
// Pass criteria:
//   - Each object traces a visually distinct Lissajous path
//   - Motion is smooth and continuous
//   - Colors match the expected colors above
//   - No crash for 10+ seconds

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

// Small square as "point" tracer
static const std::vector<Vertex> dotVertices = {
    {{-0.05f, -0.05f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.05f, -0.05f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.05f,  0.05f}, {1.0f, 1.0f, 1.0f}},
    {{-0.05f,  0.05f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> dotIndices = {0, 1, 2, 2, 3, 0};

class LissajousTest {
public:
    void run() {
        init();
        mainLoop();
        cleanup();
        std::cout << "Test completed successfully." << std::endl;
    }

private:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 800;

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

        window_ = std::make_unique<Window>(WIDTH, HEIGHT, "Test: Lissajous Figures");
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
        if (std::filesystem::exists("Release/scripts/test_vis_lissajous")) {
            scriptDir = "Release/scripts/test_vis_lissajous/";
            std::cout << "[DEBUG] Using Release/scripts/" << std::endl;
        } else if (std::filesystem::exists("Debug/scripts/test_vis_lissajous")) {
            scriptDir = "Debug/scripts/test_vis_lissajous/";
            std::cout << "[DEBUG] Using Debug/scripts/" << std::endl;
        } else {
            scriptDir = "scripts/test_vis_lissajous/";
            std::cout << "[DEBUG] Using scripts/ (fallback)" << std::endl;
        }

        // Create 6 Lissajous objects
        for (int i = 1; i <= 6; ++i) {
            std::string idx = std::to_string(i);
            RenderObjectDesc d;
            d.vertices    = dotVertices;
            d.indices     = dotIndices;
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        createSyncObjects();

        std::cout << "Initialization OK: 6 Lissajous objects created." << std::endl;
        std::cout << "Obj1(Red)   1:1  circle/ellipse" << std::endl;
        std::cout << "Obj2(Green) 1:2  figure-8" << std::endl;
        std::cout << "Obj3(Blue)  1:3  three-lobe" << std::endl;
        std::cout << "Obj4(Yellow)2:3  butterfly" << std::endl;
        std::cout << "Obj5(Magenta)3:4 complex curve" << std::endl;
        std::cout << "Obj6(Cyan)  3:5  star-like" << std::endl;
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
        appInfo.pApplicationName   = "LissajousTest";
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
    std::cout << "=== TEST: test_vis_lissajous ===" << std::endl;
    std::cout << "=== Visual Test: Lissajous Figures ===" << std::endl;
    std::cout << "Purpose: Verify sin/cos compound math in mmrsl scripts" << std::endl;
    std::cout << std::endl;
    std::cout << "PASS criteria:" << std::endl;
    std::cout << "  - 6 objects each trace a visually distinct Lissajous path" << std::endl;
    std::cout << "  - Smooth continuous motion" << std::endl;
    std::cout << "  - Correct colors (red, green, blue, yellow, magenta, cyan)" << std::endl;
    std::cout << std::endl;

    try {
        LissajousTest test;
        test.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
