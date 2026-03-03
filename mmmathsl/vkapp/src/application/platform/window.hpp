#pragma once

#include <glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>
#include <functional>

namespace vkapp {

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void pollEvents() const;
    
    vk::SurfaceKHR createSurface(vk::Instance instance);
    std::vector<const char*> getRequiredExtensions() const;
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    GLFWwindow* getHandle() const { return window_; }
    
    void setResizeCallback(std::function<void(int, int)> callback);

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    
    GLFWwindow* window_ = nullptr;
    int width_;
    int height_;
    std::function<void(int, int)> resizeCallback_;
};

} // namespace vkapp
