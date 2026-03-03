#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

namespace vkapp {

class VulkanDevice;

class VulkanPipeline {
public:
    VulkanPipeline(VulkanDevice* device, vk::RenderPass renderPass, 
                   const std::string& vertShaderPath, const std::string& fragShaderPath,
                   vk::Extent2D extent, vk::DescriptorSetLayout descriptorSetLayout = nullptr);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    vk::Pipeline getPipeline() const { return pipeline_; }
    vk::PipelineLayout getLayout() const { return layout_; }
    vk::RenderPass getRenderPass() const { return renderPass_; }

private:
    void createRenderPass();
    void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, 
                                vk::Extent2D extent, vk::DescriptorSetLayout descriptorSetLayout);
    vk::ShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);

    VulkanDevice* device_;
    vk::RenderPass renderPass_;
    vk::PipelineLayout layout_;
    vk::Pipeline pipeline_;
};

} // namespace vkapp
