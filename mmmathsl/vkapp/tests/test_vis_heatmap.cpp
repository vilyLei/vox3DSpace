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
#include <string>
using namespace vkapp;

static const std::vector<Vertex> rectVertices = {
    {{-0.1f, -0.1f}, {1.f, 1.f, 1.f}},
    {{ 0.1f, -0.1f}, {1.f, 1.f, 1.f}},
    {{ 0.1f,  0.1f}, {1.f, 1.f, 1.f}},
    {{-0.1f,  0.1f}, {1.f, 1.f, 1.f}}
};
static const std::vector<uint16_t> rectIndices = {0,1,2,2,3,0};

class Test_test_vis_heatmap {
public:
    void run() { init(); mainLoop(); cleanup(); std::cout << "Test completed." << std::endl; }
private:
    std::unique_ptr<Window>          window_;
    vk::Instance                     instance_;
    vk::SurfaceKHR                   surface_;
    std::unique_ptr<VulkanDevice>    device_;
    std::unique_ptr<VulkanSwapChain> swapChain_;
    std::unique_ptr<VulkanPipeline>  pipeline_;
    std::unique_ptr<Renderer>        renderer_;
    vk::RenderPass                   renderPass_;
    vk::Semaphore                    imageAvailable_, renderFinished_;
    vk::Fence                        inFlightFence_;
    bool                             framebufferResized_ = false;
    Scene                            scene_;

    void init() {
        std::cout << "[DEBUG] CWD: " << std::filesystem::current_path() << std::endl;
        createInstance();
        window_ = std::make_unique<Window>(700, 700, "Heatmap Color Mapping");
        window_->setResizeCallback([this](int,int){ framebufferResized_=true; });
        surface_ = window_->createSurface(instance_);
        device_    = std::make_unique<VulkanDevice>(instance_, surface_);
        swapChain_ = std::make_unique<VulkanSwapChain>(device_.get(), surface_, 700, 700);
        renderPass_ = createRenderPass();
        swapChain_->createFramebuffers(renderPass_);
        VulkanUniformBuffer tempUbo(device_.get());
        pipeline_ = std::make_unique<VulkanPipeline>(device_.get(), renderPass_,
            "shaders/rect.vert.spv","shaders/rect.frag.spv",
            swapChain_->getExtent(), tempUbo.getDescriptorSetLayout());
        renderer_ = std::make_unique<Renderer>(device_.get(), swapChain_.get(), pipeline_.get());

        std::string scriptDir;
        if (std::filesystem::exists("Release/scripts/test_vis_heatmap"))
            scriptDir = "Release/scripts/test_vis_heatmap/";
        else if (std::filesystem::exists("Debug/scripts/test_vis_heatmap"))
            scriptDir = "Debug/scripts/test_vis_heatmap/";
        else
            scriptDir = "scripts/test_vis_heatmap/";
        std::cout << "[DEBUG] scriptDir: " << scriptDir << std::endl;

        {
            std::string idx = "1";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {-0.5f, -0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "2";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.0f, -0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "3";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.5f, -0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "4";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {-0.5f, 0.0f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "5";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.0f, 0.0f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "6";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.5f, 0.0f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "7";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {-0.5f, 0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "8";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.0f, 0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }
        {
            std::string idx = "9";
            RenderObjectDesc d;
            d.vertices    = rectVertices;
            d.indices     = rectIndices;
            d.position    = {0.5f, 0.5f};
            d.scriptPaths = {scriptDir + "obj" + idx + "_color.glsl",
                             scriptDir + "obj" + idx + "_rotation.glsl",
                             scriptDir + "obj" + idx + "_position.glsl",
                             scriptDir + "obj" + idx + "_scale.glsl"};
            scene_.addObject(std::make_unique<RenderObject>(device_.get(), d));
        }

        createSyncObjects();
        std::cout << "Init OK: 9 objects created." << std::endl;
        std::cout << "Close window to end." << std::endl;
    }

    vk::RenderPass createRenderPass() {
        vk::AttachmentDescription ca{}; ca.format=swapChain_->getImageFormat();
        ca.samples=vk::SampleCountFlagBits::e1; ca.loadOp=vk::AttachmentLoadOp::eClear;
        ca.storeOp=vk::AttachmentStoreOp::eStore;
        ca.stencilLoadOp=vk::AttachmentLoadOp::eDontCare;
        ca.stencilStoreOp=vk::AttachmentStoreOp::eDontCare;
        ca.initialLayout=vk::ImageLayout::eUndefined;
        ca.finalLayout=vk::ImageLayout::ePresentSrcKHR;
        vk::AttachmentReference cr{}; cr.attachment=0;
        cr.layout=vk::ImageLayout::eColorAttachmentOptimal;
        vk::SubpassDescription sp{}; sp.pipelineBindPoint=vk::PipelineBindPoint::eGraphics;
        sp.colorAttachmentCount=1; sp.pColorAttachments=&cr;
        vk::SubpassDependency dep{}; dep.srcSubpass=VK_SUBPASS_EXTERNAL;
        dep.srcStageMask=vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.srcAccessMask=vk::AccessFlagBits::eNone;
        dep.dstStageMask=vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dep.dstAccessMask=vk::AccessFlagBits::eColorAttachmentWrite;
        vk::RenderPassCreateInfo info{}; info.attachmentCount=1; info.pAttachments=&ca;
        info.subpassCount=1; info.pSubpasses=&sp; info.dependencyCount=1; info.pDependencies=&dep;
        return device_->getDevice().createRenderPass(info);
    }

    void createSyncObjects() {
        vk::SemaphoreCreateInfo si{};
        imageAvailable_=device_->getDevice().createSemaphore(si);
        renderFinished_=device_->getDevice().createSemaphore(si);
        vk::FenceCreateInfo fi{}; fi.flags=vk::FenceCreateFlagBits::eSignaled;
        inFlightFence_=device_->getDevice().createFence(fi);
    }

    void mainLoop() {
        auto start = std::chrono::high_resolution_clock::now();
        while (!window_->shouldClose()) { window_->pollEvents(); drawFrame(start); }
        device_->getDevice().waitIdle();
    }

    void drawFrame(const std::chrono::high_resolution_clock::time_point& start) {
        device_->getDevice().waitForFences(inFlightFence_, VK_TRUE, UINT64_MAX);
        bool outOfDate=false;
        uint32_t idx=swapChain_->acquireNextImage(imageAvailable_, outOfDate);
        if (outOfDate||framebufferResized_) { recreateSwapChain(); return; }
        device_->getDevice().resetFences(inFlightFence_);
        float t=std::chrono::duration<float>(std::chrono::high_resolution_clock::now()-start).count();
        float aspect=(float)window_->getWidth()/(float)window_->getHeight();
        glm::mat4 view=glm::lookAt(glm::vec3(0,0,2),glm::vec3(0),glm::vec3(0,1,0));
        glm::mat4 proj=glm::perspective(glm::radians(45.f),aspect,0.1f,100.f); proj[1][1]*=-1;
        scene_.update(t,view,proj);
        renderer_->beginFrame(idx);
        scene_.drawAll(renderer_->getCommandBuffer(),pipeline_->getLayout());
        renderer_->endFrame();
        renderer_->submit(imageAvailable_,renderFinished_,inFlightFence_);
        swapChain_->present(idx,renderFinished_,outOfDate);
        if (outOfDate||framebufferResized_) recreateSwapChain();
    }

    void recreateSwapChain() {
        device_->getDevice().waitIdle();
        swapChain_->recreate(window_->getWidth(),window_->getHeight(),renderPass_);
        framebufferResized_=false;
    }

    void cleanup() {
        device_->getDevice().waitIdle();
        device_->getDevice().destroyFence(inFlightFence_);
        device_->getDevice().destroySemaphore(renderFinished_);
        device_->getDevice().destroySemaphore(imageAvailable_);
        scene_.clear();
        renderer_.reset();
        device_->getDevice().destroyRenderPass(renderPass_);
        pipeline_.reset(); swapChain_.reset(); device_.reset();
        instance_.destroySurfaceKHR(surface_); instance_.destroy();
        window_.reset(); glfwTerminate();
    }

    void createInstance() {
        vk::ApplicationInfo ai{}; ai.pApplicationName="test_vis_heatmap"; ai.apiVersion=VK_API_VERSION_1_0;
        glfwInit(); uint32_t cnt=0; const char** exts=glfwGetRequiredInstanceExtensions(&cnt);
        std::vector<const char*> extensions(exts,exts+cnt);
        vk::InstanceCreateInfo ci{}; ci.pApplicationInfo=&ai;
        ci.enabledExtensionCount=(uint32_t)extensions.size(); ci.ppEnabledExtensionNames=extensions.data();
        instance_=vk::createInstance(ci);
    }
};

int main() {
    std::cout << "=== TEST: test_vis_heatmap ===" << std::endl;
    std::cout << "Purpose: Verify smoothstep, sqrt, clamp heat color mapping" << std::endl;
    try { Test_test_vis_heatmap test; test.run(); return 0; }
    catch (const std::exception& e) { std::cerr << "FAILED: " << e.what() << std::endl; return 1; }
}