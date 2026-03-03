#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace vkapp {

class VulkanDevice;

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanUniformBuffer {
public:
    VulkanUniformBuffer(VulkanDevice* device);
    ~VulkanUniformBuffer();

    VulkanUniformBuffer(const VulkanUniformBuffer&) = delete;
    VulkanUniformBuffer& operator=(const VulkanUniformBuffer&) = delete;

    void update(const UniformBufferObject& ubo);
    
    vk::Buffer getBuffer() const { return buffer_; }
    vk::DescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout_; }
    vk::DescriptorSet getDescriptorSet() const { return descriptorSet_; }

private:
    void createBuffer();
    void createDescriptorSetLayout();
    void createDescriptorSet();

    VulkanDevice* device_;
    
    vk::Buffer buffer_;
    vk::DeviceMemory bufferMemory_;
    void* mappedMemory_ = nullptr;
    
    vk::DescriptorSetLayout descriptorSetLayout_;
    vk::DescriptorPool descriptorPool_;
    vk::DescriptorSet descriptorSet_;
};

} // namespace vkapp
