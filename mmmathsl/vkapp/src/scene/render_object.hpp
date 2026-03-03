#pragma once

#include "rhi/vulkan/vulkan_device.hpp"
#include "rhi/vulkan/vulkan_uniform_buffer.hpp"
#include "core/types.hpp"

#include <mmrsl/mmrsl.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>
#include <memory>

namespace vkapp {

// Descriptor for constructing a RenderObject
struct RenderObjectDesc {
    std::vector<Vertex>   vertices;
    std::vector<uint16_t> indices;
    glm::vec2             position   = {0.0f, 0.0f}; // fallback when positionScript is empty
    float                 scale      = 1.0f;
    std::string           colorScript;    // mmrsl: vec4 f(float t)
    std::string           rotationScript; // mmrsl: float f(float t)
    std::string           positionScript; // mmrsl: vec2 f(float t) -> world XY (optional)
};

// A self-contained renderable object:
//   - owns its own vertex/index buffers
//   - owns its own UniformBuffer (model matrix + color tint)
//   - owns its own mmrsl script parsers
class RenderObject {
public:
    RenderObject(VulkanDevice* device, const RenderObjectDesc& desc);
    ~RenderObject();

    RenderObject(const RenderObject&) = delete;
    RenderObject& operator=(const RenderObject&) = delete;

    // Update UBO each frame: runs scripts, builds model matrix
    void update(float t, const glm::mat4& view, const glm::mat4& proj);

    // Record draw commands into an already-started render pass
    void draw(vk::CommandBuffer cmd, vk::PipelineLayout layout) const;

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint16_t>& indices);
    void copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);

    VulkanDevice*                        device_;
    glm::vec2                            position_;
    float                                scale_;

    std::unique_ptr<VulkanUniformBuffer> uniformBuffer_;

    vk::Buffer                           vertexBuffer_;
    vk::DeviceMemory                     vertexBufferMemory_;
    vk::Buffer                           indexBuffer_;
    vk::DeviceMemory                     indexBufferMemory_;
    uint32_t                             indexCount_ = 0;

    // Command pool needed for buffer copy operations
    vk::CommandPool                      commandPool_;

    mmrsl::HighPerfParser                colorParser_;
    mmrsl::HighPerfParser                rotParser_;
    mmrsl::HighPerfParser                posParser_;
    bool                                 hasPositionScript_ = false;
};

} // namespace vkapp
