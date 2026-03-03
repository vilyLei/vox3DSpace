#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <memory>

namespace vkapp {

class VulkanDevice;
class VulkanSwapChain;
class VulkanPipeline;

class Renderer {
public:
    Renderer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Begin render pass + bind pipeline. Call before scene_.drawAll()
    void beginFrame(uint32_t imageIndex);
    // End render pass and close command buffer
    void endFrame();
    // Submit the command buffer to the graphics queue
    void submit(vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::Fence fence);

    // Expose current command buffer so RenderObjects can record into it
    vk::CommandBuffer getCommandBuffer() const { return commandBuffers_[0]; }

private:
    void createCommandPool();
    void createCommandBuffers();

    VulkanDevice*    device_;
    VulkanSwapChain* swapChain_;
    VulkanPipeline*  pipeline_;

    vk::CommandPool                commandPool_;
    std::vector<vk::CommandBuffer> commandBuffers_;

    uint32_t currentImageIndex_ = 0;
};

} // namespace vkapp
