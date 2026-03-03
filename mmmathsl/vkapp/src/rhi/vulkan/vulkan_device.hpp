#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>
#include <set>
#include <vector>

namespace vkapp {

class Window;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanDevice {
public:
    VulkanDevice(vk::Instance instance, vk::SurfaceKHR surface);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    vk::Device getDevice() const { return device_; }
    vk::PhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    vk::Queue getGraphicsQueue() const { return graphicsQueue_; }
    vk::Queue getPresentQueue() const { return presentQueue_; }
    
    QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices_; }
    
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
    
    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, 
                      vk::MemoryPropertyFlags properties,
                      vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) const;

private:
    void pickPhysicalDevice();
    void createLogicalDevice();
    
    bool isDeviceSuitable(vk::PhysicalDevice device);
    bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

    vk::Instance instance_;
    vk::SurfaceKHR surface_;
    vk::PhysicalDevice physicalDevice_;
    vk::Device device_;
    
    vk::Queue graphicsQueue_;
    vk::Queue presentQueue_;
    QueueFamilyIndices queueFamilyIndices_;
    
    const std::vector<const char*> deviceExtensions_ = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

} // namespace vkapp
