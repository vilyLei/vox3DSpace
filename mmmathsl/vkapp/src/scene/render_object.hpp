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
#include <filesystem>

namespace vkapp {

// Script file paths for hot-reload support
struct ScriptPaths {
    std::string colorScript;    // path to .glsl file
    std::string rotationScript; // path to .glsl file
    std::string positionScript; // path to .glsl file (optional)
    std::string scaleScript;    // path to .glsl file (optional, float multiplier on base scale)
};

// Descriptor for constructing a RenderObject
struct RenderObjectDesc {
    std::vector<Vertex>   vertices;
    std::vector<uint16_t> indices;
    glm::vec2             position   = {0.0f, 0.0f}; // fallback when positionScript is empty
    float                 scale      = 1.0f;

    // Inline scripts (used if scriptPaths are empty)
    std::string           colorScript;    // mmrsl: vec4 f(float t)
    std::string           rotationScript; // mmrsl: float f(float t)
    std::string           positionScript; // mmrsl: vec2 f(float t) -> world XY offset (optional)
    std::string           scaleScript;    // mmrsl: float f(float t) -> scale multiplier (optional)

    // File-based scripts (takes precedence over inline scripts if set)
    ScriptPaths           scriptPaths;
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

    // Reload and recompile scripts if source files have changed.
    // Returns true if any script was reloaded.
    bool reloadIfChanged();

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

    mmrsl::BytecodeParser                colorParser_;
    mmrsl::BytecodeParser                rotParser_;
    mmrsl::BytecodeParser                posParser_;
    mmrsl::BytecodeParser                scaleParser_;
    bool                                 hasPositionScript_ = false;
    bool                                 hasScaleScript_    = false;

    // File paths and modification times for hot-reload
    ScriptPaths                          scriptPaths_;
    std::filesystem::file_time_type      colorMtime_;
    std::filesystem::file_time_type      rotMtime_;
    std::filesystem::file_time_type      posMtime_;
    std::filesystem::file_time_type      scaleMtime_;
    bool                                 hasPositionPath_ = false;
    bool                                 hasScalePath_    = false;
};

} // namespace vkapp
