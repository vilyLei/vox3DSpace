// Test: Vulkan device initialization
#include "application/platform/window.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include <iostream>
#include <cassert>

using namespace vkapp;

class TestInstance {
public:
    TestInstance() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Test";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
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
        
        instance_ = vk::createInstance(createInfo);
    }
    
    ~TestInstance() {
        instance_.destroy();
        glfwTerminate();
    }
    
    vk::Instance get() const { return instance_; }
    
private:
    vk::Instance instance_;
};

void testDeviceCreation() {
    std::cout << "Test: Device creation..." << std::endl;
    
    TestInstance testInstance;
    Window window(800, 600, "Test Device");
    vk::SurfaceKHR surface = window.createSurface(testInstance.get());
    
    {
        VulkanDevice device(testInstance.get(), surface);
        assert(device.getDevice());
        assert(device.getPhysicalDevice());
        assert(device.getGraphicsQueue());
        assert(device.getPresentQueue());
        
        auto indices = device.getQueueFamilyIndices();
        assert(indices.isComplete());
        
        std::cout << "  PASSED: Device created successfully" << std::endl;
        std::cout << "    - Graphics queue family: " << indices.graphicsFamily.value() << std::endl;
        std::cout << "    - Present queue family: " << indices.presentFamily.value() << std::endl;
    }
    
    testInstance.get().destroySurfaceKHR(surface);
    std::cout << "  PASSED: Device cleanup successful" << std::endl;
}

void testMemoryTypeQuery() {
    std::cout << "Test: Memory type query..." << std::endl;
    
    TestInstance testInstance;
    Window window(800, 600, "Test Memory");
    vk::SurfaceKHR surface = window.createSurface(testInstance.get());
    
    {
        VulkanDevice device(testInstance.get(), surface);
        
        // Query memory type for device local buffer
        uint32_t memoryType = device.findMemoryType(
            ~0u, // All memory types
            vk::MemoryPropertyFlagBits::eDeviceLocal
        );
        
        std::cout << "  PASSED: Found device local memory type: " << memoryType << std::endl;
    }
    
    testInstance.get().destroySurfaceKHR(surface);
}

int main() {
    std::cout << "=== Device Tests ===" << std::endl;
    
    try {
        testDeviceCreation();
        testMemoryTypeQuery();
        
        std::cout << "\nAll tests PASSED!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
