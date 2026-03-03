// Test: Scene with per-object independent motion, color and rotation scripts
//
// Three objects, each fully driven by its own mmrsl scripts:
//
//  Object 1 - Rectangle: circular orbit + rainbow color + clockwise 90 deg/sec
//  Object 2 - Triangle:  horizontal oscillation + blue-white + counter-clockwise 45 deg/sec
//  Object 3 - Rectangle: Lissajous (figure-8) + red-green + fast spin 180 deg/sec
//
// Expected result: Three shapes moving independently with different trajectories and colors.
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
// Object 1 - Rectangle: circular orbit, rainbow color, CW 90 deg/sec
// ---------------------------------------------------------------------------

static const char* OBJ1_POS_SCRIPT = R"(
vec2 positionScript(float t) {
    return vec2(0.55 * cos(t), 0.35 * sin(t));
}
)";

static const char* OBJ1_COLOR_SCRIPT = R"(
vec4 colorScript(float t) {
    float r = 0.5 + 0.5 * sin(t);
    float g = 0.5 + 0.5 * sin(t + 2.094);
    float b = 0.5 + 0.5 * sin(t + 4.189);
    return vec4(r, g, b, 1.0);
}
)";

static const char* OBJ1_ROT_SCRIPT = R"(
float rotationScript(float t) {
    return t * 1.5708;
}
)";

// ---------------------------------------------------------------------------
// Object 2 - Triangle: horizontal oscillation, blue-white, CCW 45 deg/sec
// ---------------------------------------------------------------------------

static const char* OBJ2_POS_SCRIPT = R"(
vec2 positionScript(float t) {
    return vec2(0.65 * sin(t * 1.5), 0.0);
}
)";

static const char* OBJ2_COLOR_SCRIPT = R"(
vec4 colorScript(float t) {
    float v = 0.5 + 0.5 * sin(t * 2.0);
    return vec4(v, v, 1.0, 1.0);
}
)";

static const char* OBJ2_ROT_SCRIPT = R"(
float rotationScript(float t) {
    return -(t * 0.7854);
}
)";

// ---------------------------------------------------------------------------
// Object 3 - Rectangle: Lissajous figure-8, red-green oscillation, fast spin
// ---------------------------------------------------------------------------

static const char* OBJ3_POS_SCRIPT = R"(
vec2 positionScript(float t) {
    return vec2(0.5 * sin(t * 0.9), 0.4 * sin(t * 1.8));
}
)";

static const char* OBJ3_COLOR_SCRIPT = R"(
vec4 colorScript(float t) {
    float r = 0.5 + 0.5 * sin(t * 1.5);
    float g = 0.5 + 0.5 * sin(t * 1.5 + 3.1416);
    return vec4(r, g, 0.2, 1.0);
}
)";

static const char* OBJ3_ROT_SCRIPT = R"(
float rotationScript(float t) {
    return t * 3.1416;
}
)";

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

    void init() {
        createInstance();

        window_ = std::make_unique<Window>(WIDTH, HEIGHT, "Test: Per-Object Motion Scripts");
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
            d.vertices        = rectVertices;
            d.indices         = rectIndices;
            d.colorScript     = OBJ1_COLOR_SCRIPT;
            d.rotationScript  = OBJ1_ROT_SCRIPT;
            d.positionScript  = OBJ1_POS_SCRIPT;
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        // Object 2: triangle, horizontal oscillation
        {
            RenderObjectDesc d;
            d.vertices        = triVertices;
            d.indices         = triIndices;
            d.colorScript     = OBJ2_COLOR_SCRIPT;
            d.rotationScript  = OBJ2_ROT_SCRIPT;
            d.positionScript  = OBJ2_POS_SCRIPT;
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        // Object 3: rectangle, Lissajous figure-8
        {
            RenderObjectDesc d;
            d.vertices        = rectVertices;
            d.indices         = rectIndices;
            d.colorScript     = OBJ3_COLOR_SCRIPT;
            d.rotationScript  = OBJ3_ROT_SCRIPT;
            d.positionScript  = OBJ3_POS_SCRIPT;
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        createSyncObjects();

        std::cout << "Initialization OK. Scene has "
                  << scene_.objectCount() << " objects." << std::endl;
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
    std::cout << "=== Per-Object Motion Script Test ===" << std::endl;
    std::cout << "Object 1 (rect):     circular orbit    | rainbow color    | CW  90 deg/sec" << std::endl;
    std::cout << "Object 2 (triangle): horizontal bounce | blue-white pulse | CCW 45 deg/sec" << std::endl;
    std::cout << "Object 3 (rect):     figure-8 Lissajous| red-green cycle  | fast 180 deg/sec" << std::endl;
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
