#include "vulkan_uniform_buffer.hpp"
#include "vulkan_device.hpp"

#include <cstring>

namespace vkapp {

VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device) : device_(device) {
    createBuffer();
    createDescriptorSetLayout();
    createDescriptorSet();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    device_->getDevice().destroyDescriptorPool(descriptorPool_);
    device_->getDevice().destroyDescriptorSetLayout(descriptorSetLayout_);
    device_->getDevice().unmapMemory(bufferMemory_);
    device_->getDevice().destroyBuffer(buffer_);
    device_->getDevice().freeMemory(bufferMemory_);
}

void VulkanUniformBuffer::createBuffer() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
    
    device_->createBuffer(bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        buffer_, bufferMemory_);
    
    mappedMemory_ = device_->getDevice().mapMemory(bufferMemory_, 0, bufferSize);
}

void VulkanUniformBuffer::createDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;
    
    descriptorSetLayout_ = device_->getDevice().createDescriptorSetLayout(layoutInfo);
}

void VulkanUniformBuffer::createDescriptorSet() {
    vk::DescriptorPoolSize poolSize{};
    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = 1;
    
    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;
    
    descriptorPool_ = device_->getDevice().createDescriptorPool(poolInfo);
    
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool_;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout_;
    
    descriptorSet_ = device_->getDevice().allocateDescriptorSets(allocInfo)[0];
    
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer_;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);
    
    vk::WriteDescriptorSet descriptorWrite{};
    descriptorWrite.dstSet = descriptorSet_;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    
    device_->getDevice().updateDescriptorSets(descriptorWrite, nullptr);
}

void VulkanUniformBuffer::update(const UniformBufferObject& ubo) {
    memcpy(mappedMemory_, &ubo, sizeof(ubo));
}

} // namespace vkapp
