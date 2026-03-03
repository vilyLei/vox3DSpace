#include <vulkan/vulkan.hpp>

#include "application/platform/window.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_pipeline.hpp"
#include "rhi/vulkan/vulkan_uniform_buffer.hpp"
#include "renderer/renderer.hpp"
#include "core/types.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <chrono>

namespace vkapp {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

class VulkanApp {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    std::unique_ptr<Window> window_;
    
    vk::Instance instance_;
    vk::DebugUtilsMessengerEXT debugMessenger_;
    vk::SurfaceKHR surface_;
    
    std::unique_ptr<VulkanDevice> device_;
    std::unique_ptr<VulkanSwapChain> swapChain_;
    std::unique_ptr<VulkanUniformBuffer> uniformBuffer_;
    std::unique_ptr<VulkanPipeline> pipeline_;
    std::unique_ptr<Renderer> renderer_;
    
    vk::Semaphore imageAvailableSemaphore_;
    vk::Semaphore renderFinishedSemaphore_;
    vk::Fence inFlightFence_;
    
    bool framebufferResized_ = false;

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        
        window_ = std::make_unique<Window>(WINDOW_WIDTH, WINDOW_HEIGHT, "Red Rectangle");
        window_->setResizeCallback([this](int width, int height) {
            framebufferResized_ = true;
        });
        surface_ = window_->createSurface(instance_);
        
        device_ = std::make_unique<VulkanDevice>(instance_, surface_);
        swapChain_ = std::make_unique<VulkanSwapChain>(device_.get(), surface_, WINDOW_WIDTH, WINDOW_HEIGHT);
        
        // Create render pass first
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = swapChain_->getImageFormat();
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
        
        vk::AttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
        
        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        
        vk::SubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.srcAccessMask = vk::AccessFlagBits::eNone;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        
        vk::RenderPass renderPass = device_->getDevice().createRenderPass(renderPassInfo);
        
        // Create framebuffers after render pass
        swapChain_->createFramebuffers(renderPass);
        
        // Create uniform buffer
        uniformBuffer_ = std::make_unique<VulkanUniformBuffer>(device_.get());
        
        pipeline_ = std::make_unique<VulkanPipeline>(device_.get(), renderPass,
            "shaders/rect.vert.spv", "shaders/rect.frag.spv", swapChain_->getExtent(),
            uniformBuffer_->getDescriptorSetLayout());
        
        renderer_ = std::make_unique<Renderer>(device_.get(), swapChain_.get(), pipeline_.get());
        renderer_->createVertexBuffer(vertices);
        renderer_->createIndexBuffer(indices);
        
        createSyncObjects();
    }

    void createInstance() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Red Rectangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        
        // Get extensions from GLFW
        glfwInit();
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;
        
        instance_ = vk::createInstance(createInfo);
    }

    void setupDebugMessenger() {
        // Debug messenger setup omitted for simplicity
    }

    void createSyncObjects() {
        vk::SemaphoreCreateInfo semaphoreInfo{};
        imageAvailableSemaphore_ = device_->getDevice().createSemaphore(semaphoreInfo);
        renderFinishedSemaphore_ = device_->getDevice().createSemaphore(semaphoreInfo);
        
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        inFlightFence_ = device_->getDevice().createFence(fenceInfo);
    }

    void mainLoop() {
        while (!window_->shouldClose()) {
            window_->pollEvents();
            drawFrame();
        }
        
        device_->getDevice().waitIdle();
    }

    void drawFrame() {
        device_->getDevice().waitForFences(inFlightFence_, VK_TRUE, UINT64_MAX);
        
        bool outOfDate = false;
        uint32_t imageIndex = swapChain_->acquireNextImage(imageAvailableSemaphore_, outOfDate);
        
        if (outOfDate || framebufferResized_) {
            recreateSwapChain();
            return;
        }
        
        device_->getDevice().resetFences(inFlightFence_);
        
        // Update uniform buffer with rotation
        updateUniformBuffer();
        
        renderer_->beginFrame(imageIndex, uniformBuffer_->getDescriptorSet());
        renderer_->draw();
        renderer_->endFrame();
        
        renderer_->submit(imageAvailableSemaphore_, renderFinishedSemaphore_, inFlightFence_);
        swapChain_->present(imageIndex, renderFinishedSemaphore_, outOfDate);
        
        if (outOfDate || framebufferResized_) {
            recreateSwapChain();
        }
    }
    
    void recreateSwapChain() {
        device_->getDevice().waitIdle();
        
        swapChain_->recreate(window_->getWidth(), window_->getHeight(), pipeline_->getRenderPass());
        framebufferResized_ = false;
    }
    
    void updateUniformBuffer() {
        static auto startTime = std::chrono::high_resolution_clock::now();
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        
        UniformBufferObject ubo{};
        // Model: rotate around Z axis
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // View: camera at (0, 0, 2) looking at origin
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // Proj: perspective with reasonable FOV
        ubo.proj = glm::perspective(glm::radians(45.0f), 
            static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 100.0f);
        ubo.proj[1][1] *= -1; // Flip Y for Vulkan
        
        uniformBuffer_->update(ubo);
    }

    void cleanup() {
        device_->getDevice().waitIdle();
        
        device_->getDevice().destroyFence(inFlightFence_);
        device_->getDevice().destroySemaphore(renderFinishedSemaphore_);
        device_->getDevice().destroySemaphore(imageAvailableSemaphore_);
        
        renderer_.reset();
        uniformBuffer_.reset();
        device_->getDevice().destroyRenderPass(pipeline_->getRenderPass());
        pipeline_.reset();
        swapChain_.reset();
        device_.reset();
        
        instance_.destroySurfaceKHR(surface_);
        instance_.destroy();
        
        window_.reset();
        glfwTerminate();
    }
};

} // namespace vkapp

int main() {
    try {
        vkapp::VulkanApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
