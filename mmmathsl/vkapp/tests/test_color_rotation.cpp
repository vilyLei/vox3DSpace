// Test: Scene with per-object independent motion, color and rotation scripts (file-based)
//
// Three objects, each fully driven by its own mmrsl scripts loaded from .glsl files:
//
//  Object 1 - Rectangle: circular orbit + rainbow color + clockwise 90 deg/sec
//  Object 2 - Triangle:  horizontal oscillation + blue-white + counter-clockwise 45 deg/sec
//  Object 3 - Rectangle: Lissajous (figure-8) + red-green + fast spin 180 deg/sec
//
// Scripts are loaded from vkapp/scripts/ directory and support hot-reload.
// Expected result: Three shapes moving independently with different trajectories and colors.
// Try editing scripts/*.glsl while the program is running to see changes immediately.
// Close the window to exit.

#define GLFW_INCLUDE_VULKAN
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

// ---------------------------------------------------------------------------
// Mesh data
// ---------------------------------------------------------------------------

static const std::vector<Vertex> rectVertices = {
    {{-0.3f, -0.3f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.3f, -0.3f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}},
    {{-0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> rectIndices = {0, 1, 2, 2, 3, 0};

static const std::vector<Vertex> triVertices = {
    {{ 0.0f, -0.3f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}},
    {{-0.3f,  0.3f}, {1.0f, 1.0f, 1.0f}}
};
static const std::vector<uint16_t> triIndices = {0, 1, 2};

// ---------------------------------------------------------------------------
// Test class
// ---------------------------------------------------------------------------

class SceneMotionTest {
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
    uint32_t frameCount_ = 0;

    void init() {
        createInstance();

        window_ = std::make_unique<Window>(WIDTH, HEIGHT, "Test: Script Hot-Reload");
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

        // Object 1: rectangle, circular orbit
        {
            RenderObjectDesc d;
            d.vertices       = rectVertices;
            d.indices        = rectIndices;
            d.scriptPaths    = {"scripts/obj1_color.glsl",
                                "scripts/obj1_rotation.glsl",
                                "scripts/obj1_position.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        // Object 2: triangle, horizontal oscillation
        {
            RenderObjectDesc d;
            d.vertices       = triVertices;
            d.indices        = triIndices;
            d.scriptPaths    = {"scripts/obj2_color.glsl",
                                "scripts/obj2_rotation.glsl",
                                "scripts/obj2_position.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        // Object 3: rectangle, Lissajous figure-8
        {
            RenderObjectDesc d;
            d.vertices       = rectVertices;
            d.indices        = rectIndices;
            d.scriptPaths    = {"scripts/obj3_color.glsl",
                                "scripts/obj3_rotation.glsl",
                                "scripts/obj3_position.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        createSyncObjects();

        std::cout << "Initialization OK. Scene has "
                  << scene_.objectCount() << " objects." << std::endl;
        std::cout << "Scripts loaded from scripts/ directory." << std::endl;
        std::cout << "Edit .glsl files while running to see hot-reload in action!" << std::endl;
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

        // Hot-reload: check for script changes every ~60 frames (~1 second at 60fps)
        if (frameCount_ % 60 == 0) {
            scene_.reloadScripts();
        }
        ++frameCount_;

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
        appInfo.pApplicationName   = "SceneMotionTest";
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
    std::cout << "=== TEST: test_color_rotation ===" << std::endl;
    std::cout << "=== Script Hot-Reload Test ===" << std::endl;
    std::cout << "Object 1 (rect):     circular orbit    | rainbow color    | CW  90 deg/sec" << std::endl;
    std::cout << "Object 2 (triangle): horizontal bounce | blue-white pulse | CCW 45 deg/sec" << std::endl;
    std::cout << "Object 3 (rect):     figure-8 Lissajous| red-green cycle  | fast 180 deg/sec" << std::endl;
    std::cout << std::endl;
    std::cout << "Scripts are loaded from scripts/*.glsl files." << std::endl;
    std::cout << "Try editing them while the program runs - changes apply immediately!" << std::endl;
    std::cout << "Close the window to end the test." << std::endl;

    try {
        SceneMotionTest test;
        test.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
