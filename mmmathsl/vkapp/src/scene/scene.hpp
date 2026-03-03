#pragma once

#include "render_object.hpp"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <vector>
#include <memory>

namespace vkapp {

// Scene owns a collection of RenderObjects and orchestrates
// per-frame update and draw calls.
class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void addObject(std::unique_ptr<RenderObject> obj);

    // Update all objects: runs scripts, uploads UBOs
    void update(float t, const glm::mat4& view, const glm::mat4& proj);

    // Record draw commands for every object into cmd
    void drawAll(vk::CommandBuffer cmd, vk::PipelineLayout layout) const;

    std::size_t objectCount() const { return objects_.size(); }

private:
    std::vector<std::unique_ptr<RenderObject>> objects_;
};

} // namespace vkapp
