#include "vulkan_swapchain.hpp"
#include "vulkan_device.hpp"

#include <stdexcept>
#include <algorithm>

namespace vkapp {

VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, vk::SurfaceKHR surface, int width, int height)
    : device_(device), surface_(surface) {
    createSwapChain(width, height);
    createImageViews();
}

VulkanSwapChain::~VulkanSwapChain() {
    for (auto framebuffer : framebuffers_) {
        device_->getDevice().destroyFramebuffer(framebuffer);
    }
    for (auto imageView : imageViews_) {
        device_->getDevice().destroyImageView(imageView);
    }
    device_->getDevice().destroySwapchainKHR(swapChain_);
}

void VulkanSwapChain::createSwapChain(int width, int height) {
    SwapChainSupportDetails details = querySupportDetails();
    
    vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(details.formats);
    vk::PresentModeKHR presentMode = choosePresentMode(details.presentModes);
    vk::Extent2D extent = chooseExtent(details.capabilities, width, height);
    
    uint32_t imageCount = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
        imageCount = details.capabilities.maxImageCount;
    }
    
    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    
    QueueFamilyIndices indices = device_->getQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }
    
    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr;
    
    swapChain_ = device_->getDevice().createSwapchainKHR(createInfo);
    images_ = device_->getDevice().getSwapchainImagesKHR(swapChain_);
    imageFormat_ = surfaceFormat.format;
    extent_ = extent;
}

void VulkanSwapChain::createImageViews() {
    imageViews_.resize(images_.size());
    
    for (size_t i = 0; i < images_.size(); i++) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.image = images_[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = imageFormat_;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        imageViews_[i] = device_->getDevice().createImageView(createInfo);
    }
}

void VulkanSwapChain::createFramebuffers(vk::RenderPass renderPass) {
    if (!framebuffers_.empty()) {
        for (auto framebuffer : framebuffers_) {
            device_->getDevice().destroyFramebuffer(framebuffer);
        }
    }
    framebuffers_.resize(imageViews_.size());
    
    for (size_t i = 0; i < imageViews_.size(); i++) {
        vk::ImageView attachments[] = {imageViews_[i]};
        
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent_.width;
        framebufferInfo.height = extent_.height;
        framebufferInfo.layers = 1;
        
        framebuffers_[i] = device_->getDevice().createFramebuffer(framebufferInfo);
    }
}

uint32_t VulkanSwapChain::acquireNextImage(vk::Semaphore signalSemaphore, bool& outOfDate) {
    outOfDate = false;
    auto result = device_->getDevice().acquireNextImageKHR(swapChain_, UINT64_MAX, signalSemaphore, nullptr);
    if (result.result == vk::Result::eErrorOutOfDateKHR) {
        outOfDate = true;
        return 0;
    }
    return result.value;
}

void VulkanSwapChain::present(uint32_t imageIndex, vk::Semaphore waitSemaphore, bool& outOfDate) {
    outOfDate = false;
    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
    vk::SwapchainKHR swapChains[] = {swapChain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    auto result = device_->getPresentQueue().presentKHR(&presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        outOfDate = true;
    }
}

void VulkanSwapChain::recreate(int width, int height, vk::RenderPass renderPass) {
    device_->getDevice().waitIdle();
    
    // Cleanup old resources
    for (auto framebuffer : framebuffers_) {
        device_->getDevice().destroyFramebuffer(framebuffer);
    }
    framebuffers_.clear();
    
    for (auto imageView : imageViews_) {
        device_->getDevice().destroyImageView(imageView);
    }
    imageViews_.clear();
    
    device_->getDevice().destroySwapchainKHR(swapChain_);
    
    // Recreate
    createSwapChain(width, height);
    createImageViews();
    createFramebuffers(renderPass);
}

SwapChainSupportDetails VulkanSwapChain::querySupportDetails() {
    SwapChainSupportDetails details;
    details.capabilities = device_->getPhysicalDevice().getSurfaceCapabilitiesKHR(surface_);
    details.formats = device_->getPhysicalDevice().getSurfaceFormatsKHR(surface_);
    details.presentModes = device_->getPhysicalDevice().getSurfacePresentModesKHR(surface_);
    return details;
}

vk::SurfaceFormatKHR VulkanSwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available) {
    for (const auto& availableFormat : available) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && 
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return available[0];
}

vk::PresentModeKHR VulkanSwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR>& available) {
    for (const auto& availablePresentMode : available) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities, int width, int height) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        actualExtent.width = std::max(capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}

} // namespace vkapp
