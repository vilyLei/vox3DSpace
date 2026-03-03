#include "renderer.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_pipeline.hpp"

#include <stdexcept>

namespace vkapp {

Renderer::Renderer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline)
    : device_(device), swapChain_(swapChain), pipeline_(pipeline) {
    createCommandPool();
    createCommandBuffers();
}

Renderer::~Renderer() {
    device_->getDevice().destroyCommandPool(commandPool_);
}

void Renderer::createCommandPool() {
    QueueFamilyIndices indices = device_->getQueueFamilyIndices();

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    commandPool_ = device_->getDevice().createCommandPool(poolInfo);
}

void Renderer::createCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool        = commandPool_;
    allocInfo.level              = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    commandBuffers_ = device_->getDevice().allocateCommandBuffers(allocInfo);
}

void Renderer::beginFrame(uint32_t imageIndex) {
    currentImageIndex_ = imageIndex;
    commandBuffers_[0].reset();

    vk::CommandBufferBeginInfo beginInfo{};
    commandBuffers_[0].begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass        = pipeline_->getRenderPass();
    renderPassInfo.framebuffer       = swapChain_->getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = swapChain_->getExtent();

    vk::ClearValue clearColor{};
    clearColor.color = vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    commandBuffers_[0].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    commandBuffers_[0].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->getPipeline());
}

void Renderer::endFrame() {
    commandBuffers_[0].endRenderPass();
    commandBuffers_[0].end();
}

void Renderer::submit(vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::Fence fence) {
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::Semaphore          waitSems[]   = {waitSemaphore};
    vk::Semaphore          signalSems[] = {signalSemaphore};

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSems;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &commandBuffers_[0];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSems;

    device_->getGraphicsQueue().submit(submitInfo, fence);
}

} // namespace vkapp
