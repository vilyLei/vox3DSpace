#include "renderer.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include "rhi/vulkan/vulkan_swapchain.hpp"
#include "rhi/vulkan/vulkan_pipeline.hpp"
#include "core/types.hpp"

#include <stdexcept>
#include <cstring>

namespace vkapp {

Renderer::Renderer(VulkanDevice* device, VulkanSwapChain* swapChain, VulkanPipeline* pipeline)
    : device_(device), swapChain_(swapChain), pipeline_(pipeline) {
    createCommandPool();
    createCommandBuffers();
}

Renderer::~Renderer() {
    device_->getDevice().destroyBuffer(indexBuffer_);
    device_->getDevice().freeMemory(indexBufferMemory_);
    device_->getDevice().destroyBuffer(vertexBuffer_);
    device_->getDevice().freeMemory(vertexBufferMemory_);
    device_->getDevice().destroyCommandPool(commandPool_);
}

void Renderer::createCommandPool() {
    QueueFamilyIndices indices = device_->getQueueFamilyIndices();
    
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
    
    commandPool_ = device_->getDevice().createCommandPool(poolInfo);
}

void Renderer::createCommandBuffers() {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool_;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;
    
    commandBuffers_ = device_->getDevice().allocateCommandBuffers(allocInfo);
}

void Renderer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    device_->createBuffer(bufferSize, 
        vk::BufferUsageFlagBits::eTransferSrc, 
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);
    
    void* data = device_->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    device_->getDevice().unmapMemory(stagingBufferMemory);
    
    device_->createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertexBuffer_, vertexBufferMemory_);
    
    copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);
    
    device_->getDevice().destroyBuffer(stagingBuffer);
    device_->getDevice().freeMemory(stagingBufferMemory);
}

void Renderer::createIndexBuffer(const std::vector<uint16_t>& indices) {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    indexCount_ = static_cast<uint32_t>(indices.size());
    
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    device_->createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer, stagingBufferMemory);
    
    void* data = device_->getDevice().mapMemory(stagingBufferMemory, 0, bufferSize);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    device_->getDevice().unmapMemory(stagingBufferMemory);
    
    device_->createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        indexBuffer_, indexBufferMemory_);
    
    copyBuffer(stagingBuffer, indexBuffer_, bufferSize);
    
    device_->getDevice().destroyBuffer(stagingBuffer);
    device_->getDevice().freeMemory(stagingBufferMemory);
}

void Renderer::copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool_;
    allocInfo.commandBufferCount = 1;
    
    vk::CommandBuffer commandBuffer = device_->getDevice().allocateCommandBuffers(allocInfo)[0];
    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
    commandBuffer.begin(beginInfo);
    
    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    commandBuffer.copyBuffer(src, dst, copyRegion);
    
    commandBuffer.end();
    
    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    device_->getGraphicsQueue().submit(submitInfo);
    device_->getGraphicsQueue().waitIdle();
    
    device_->getDevice().freeCommandBuffers(commandPool_, commandBuffer);
}

void Renderer::beginFrame(uint32_t imageIndex, vk::DescriptorSet descriptorSet) {
    currentImageIndex_ = imageIndex;
    commandBuffers_[0].reset();
    
    vk::CommandBufferBeginInfo beginInfo{};
    commandBuffers_[0].begin(beginInfo);
    
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = pipeline_->getRenderPass();
    renderPassInfo.framebuffer = swapChain_->getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = swapChain_->getExtent();
    
    vk::ClearValue clearColor{};
    clearColor.color = vk::ClearColorValue{std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    commandBuffers_[0].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    
    commandBuffers_[0].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->getPipeline());
    
    if (descriptorSet) {
        commandBuffers_[0].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, 
            pipeline_->getLayout(), 0, descriptorSet, nullptr);
    }
    
    vk::Buffer vertexBuffers[] = {vertexBuffer_};
    vk::DeviceSize offsets[] = {0};
    commandBuffers_[0].bindVertexBuffers(0, 1, vertexBuffers, offsets);
    commandBuffers_[0].bindIndexBuffer(indexBuffer_, 0, vk::IndexType::eUint16);
}

void Renderer::endFrame() {
    commandBuffers_[0].endRenderPass();
    commandBuffers_[0].end();
}

void Renderer::draw() {
    commandBuffers_[0].drawIndexed(indexCount_, 1, 0, 0, 0);
}

void Renderer::submit(vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::Fence fence) {
    vk::SubmitInfo submitInfo{};
    vk::Semaphore waitSemaphores[] = {waitSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_[0];
    vk::Semaphore signalSemaphores[] = {signalSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    device_->getGraphicsQueue().submit(submitInfo, fence);
}

} // namespace vkapp
