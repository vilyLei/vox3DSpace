#ifndef VOXOL_COMPONENT_STORAGE_H
#define VOXOL_COMPONENT_STORAGE_H

#include <unordered_map>
#include <vector>
#include "Entity.h"

template<typename T>
class ComponentStorage {
public:
    std::unordered_map<Entity, T> components;

    void add(Entity e, const T& c) {
        components[e] = c;
    }

    bool has(Entity e) const {
        return components.find(e) != components.end();
    }

    T& get(Entity e) {
        return components.at(e);
    }

    const std::unordered_map<Entity, T>& all() const {
        return components;
    }
};

#endif  // VOXOL_COMPONENT_STORAGE_H