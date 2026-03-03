#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace vkapp {

class VulkanDevice;

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanDevice* device, vk::SurfaceKHR surface, int width, int height);
    ~VulkanSwapChain();

    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;

    vk::SwapchainKHR getSwapChain() const { return swapChain_; }
    vk::Format getImageFormat() const { return imageFormat_; }
    vk::Extent2D getExtent() const { return extent_; }
    size_t getImageCount() const { return images_.size(); }
    
    vk::Framebuffer getFramebuffer(size_t index) const { return framebuffers_[index]; }
    void createFramebuffers(vk::RenderPass renderPass);
    
    uint32_t acquireNextImage(vk::Semaphore signalSemaphore, bool& outOfDate);
    void present(uint32_t imageIndex, vk::Semaphore waitSemaphore, bool& outOfDate);
    
    void recreate(int width, int height, vk::RenderPass renderPass);

private:
    void createSwapChain(int width, int height);
    void createImageViews();
    
    SwapChainSupportDetails querySupportDetails();
    vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available);
    vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& available);
    vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, int width, int height);

    VulkanDevice* device_;
    vk::SurfaceKHR surface_;
    vk::SwapchainKHR swapChain_;
    
    std::vector<vk::Image> images_;
    std::vector<vk::ImageView> imageViews_;
    std::vector<vk::Framebuffer> framebuffers_;
    
    vk::Format imageFormat_;
    vk::Extent2D extent_;
};

} // namespace vkapp
