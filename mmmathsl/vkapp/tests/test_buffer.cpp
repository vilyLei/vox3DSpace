// Test: Buffer creation and management
#include "application/platform/window.hpp"
#include "rhi/vulkan/vulkan_device.hpp"
#include "core/types.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace vkapp;

class TestContext {
public:
    TestContext() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Test";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        
        glfwInit();
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        
        instance_ = vk::createInstance(createInfo);
        
        window_ = std::make_unique<Window>(800, 600, "Test Buffer");
        surface_ = window_->createSurface(instance_);
        device_ = std::make_unique<VulkanDevice>(instance_, surface_);
    }
    
    ~TestContext() {
        device_.reset();
        instance_.destroySurfaceKHR(surface_);
        window_.reset();
        instance_.destroy();
        glfwTerminate();
    }
    
    VulkanDevice* getDevice() const { return device_.get(); }
    
private:
    vk::Instance instance_;
    vk::SurfaceKHR surface_;
    std::unique_ptr<Window> window_;
    std::unique_ptr<VulkanDevice> device_;
};

void testVertexBufferCreation() {
    std::cout << "Test: Vertex buffer creation..." << std::endl;
    
    TestContext ctx;
    
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    
    ctx.getDevice()->createBuffer(
        sizeof(vertices[0]) * vertices.size(),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        buffer, memory
    );
    
    // Map and fill data
    void* data = ctx.getDevice()->getDevice().mapMemory(memory, 0, sizeof(vertices[0]) * vertices.size());
    memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
    ctx.getDevice()->getDevice().unmapMemory(memory);
    
    // Cleanup
    ctx.getDevice()->getDevice().destroyBuffer(buffer);
    ctx.getDevice()->getDevice().freeMemory(memory);
    
    std::cout << "  PASSED: Vertex buffer created and filled successfully" << std::endl;
}

void testIndexBufferCreation() {
    std::cout << "Test: Index buffer creation..." << std::endl;
    
    TestContext ctx;
    
    std::vector<uint16_t> indices = {0, 1, 2};
    
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    
    ctx.getDevice()->createBuffer(
        sizeof(indices[0]) * indices.size(),
        vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        buffer, memory
    );
    
    void* data = ctx.getDevice()->getDevice().mapMemory(memory, 0, sizeof(indices[0]) * indices.size());
    memcpy(data, indices.data(), sizeof(indices[0]) * indices.size());
    ctx.getDevice()->getDevice().unmapMemory(memory);
    
    ctx.getDevice()->getDevice().destroyBuffer(buffer);
    ctx.getDevice()->getDevice().freeMemory(memory);
    
    std::cout << "  PASSED: Index buffer created successfully" << std::endl;
}

void testUniformBufferCreation() {
    std::cout << "Test: Uniform buffer creation..." << std::endl;
    
    TestContext ctx;
    
    struct TestUBO {
        float data[4];
    };
    
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    
    ctx.getDevice()->createBuffer(
        sizeof(TestUBO),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        buffer, memory
    );
    
    // Test persistent mapping
    TestUBO* mapped = static_cast<TestUBO*>(ctx.getDevice()->getDevice().mapMemory(memory, 0, sizeof(TestUBO)));
    mapped->data[0] = 1.0f;
    mapped->data[1] = 2.0f;
    mapped->data[2] = 3.0f;
    mapped->data[3] = 4.0f;
    ctx.getDevice()->getDevice().unmapMemory(memory);
    
    ctx.getDevice()->getDevice().destroyBuffer(buffer);
    ctx.getDevice()->getDevice().freeMemory(memory);
    
    std::cout << "  PASSED: Uniform buffer created and updated successfully" << std::endl;
}

int main() {
    std::cout << "=== Buffer Tests ===" << std::endl;
    
    try {
        testVertexBufferCreation();
        testIndexBufferCreation();
        testUniformBufferCreation();
        
        std::cout << "\nAll tests PASSED!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
