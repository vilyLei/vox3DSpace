#ifndef VOXOL_TEST_COMPONENT_STORAGE_H
#define VOXOL_TEST_COMPONENT_STORAGE_H

#include <unordered_map>
#include <vector>
#include "Base/BaseDefine.h"

template<typename T>
class TestComponentStorage {
public:
    std::unordered_map<VoxolEntity, T> components;

    void add(VoxolEntity e, const T& c) {
        components[e] = c;
    }

    bool has(VoxolEntity e) const {
        return components.find(e) != components.end();
    }

    T& get(VoxolEntity e) {
        return components.at(e);
    }

    const std::unordered_map<VoxolEntity, T>& all() const {
        return components;
    }
};

#endif  // VOXOL_TEST_COMPONENT_STORAGE_H