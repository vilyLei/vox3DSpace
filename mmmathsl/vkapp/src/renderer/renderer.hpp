#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

namespace vkapp {

class VulkanDevice;
class VulkanSwapChain;
class VulkanPipeline;
class Window;

struct Vertex;

class Renderer {
public:
    Renderer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint16_t>& indices);
    
    void beginFrame(uint32_t imageIndex, vk::DescriptorSet descriptorSet = nullptr);
    void endFrame();
    void draw();
    void submit(vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::Fence fence);

private:
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    VulkanDevice* device_;
    VulkanSwapChain* swapChain_;
    VulkanPipeline* pipeline_;
    
    vk::CommandPool commandPool_;
    std::vector<vk::CommandBuffer> commandBuffers_;
    
    vk::Buffer vertexBuffer_;
    vk::DeviceMemory vertexBufferMemory_;
    vk::Buffer indexBuffer_;
    vk::DeviceMemory indexBufferMemory_;
    
    uint32_t indexCount_ = 0;
    uint32_t currentImageIndex_ = 0;
};

} // namespace vkapp
