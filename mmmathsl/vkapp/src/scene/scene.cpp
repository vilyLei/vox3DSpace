#include "scene.hpp"

namespace vkapp {

void Scene::addObject(std::unique_ptr<RenderObject> obj) {
    objects_.push_back(std::move(obj));
}

void Scene::update(float t, const glm::mat4& view, const glm::mat4& proj) {
    for (auto& obj : objects_) {
        obj->update(t, view, proj);
    }
}

void Scene::drawAll(vk::CommandBuffer cmd, vk::PipelineLayout layout) const {
    for (const auto& obj : objects_) {
        obj->draw(cmd, layout);
    }
}

void Scene::reloadScripts() {
    for (auto& obj : objects_) {
        obj->reloadIfChanged();
    }
}

} // namespace vkapp
