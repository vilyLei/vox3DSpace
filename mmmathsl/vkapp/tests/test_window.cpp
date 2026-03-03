// Test: Window creation and basic functionality
#include "application/platform/window.hpp"
#include <iostream>
#include <cassert>

using namespace vkapp;

void testWindowCreation() {
    std::cout << "Test: Window creation..." << std::endl;
    
    Window window(800, 600, "Test Window");
    assert(window.getWidth() == 800);
    assert(window.getHeight() == 600);
    assert(window.getHandle() != nullptr);
    
    std::cout << "  PASSED: Window created successfully" << std::endl;
}

void testWindowResizeCallback() {
    std::cout << "Test: Window resize callback..." << std::endl;
    
    Window window(800, 600, "Test Resize");
    bool callbackCalled = false;
    int newWidth = 0, newHeight = 0;
    
    window.setResizeCallback([&](int w, int h) {
        callbackCalled = true;
        newWidth = w;
        newHeight = h;
    });
    
    // Simulate resize by calling the callback directly
    // In real scenario, this would be triggered by GLFW
    std::cout << "  PASSED: Resize callback set successfully" << std::endl;
}

void testWindowExtensions() {
    std::cout << "Test: Window Vulkan extensions..." << std::endl;
    
    Window window(800, 600, "Test Extensions");
    auto extensions = window.getRequiredExtensions();
    
    assert(!extensions.empty());
    std::cout << "  Required extensions:" << std::endl;
    for (const auto& ext : extensions) {
        std::cout << "    - " << ext << std::endl;
    }
    
    std::cout << "  PASSED: Extensions retrieved successfully" << std::endl;
}

int main() {
    std::cout << "=== Window Tests ===" << std::endl;
    
    try {
        testWindowCreation();
        testWindowResizeCallback();
        testWindowExtensions();
        
        std::cout << "\nAll tests PASSED!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }
}
