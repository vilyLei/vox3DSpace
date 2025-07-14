#include "SlabArena.h"
#include <unordered_map>
#include <type_traits>

namespace Voxol::Base {


// 通用组件存储，使用 SlabPool 分配每个组件类型

template <typename T>
class SlabComponentStorage {
public:
    explicit SlabComponentStorage(SlabArena& arenaRef)
        : pool(arenaRef) {}

    T* add(VoxolEntity e, const T& component) {
        auto [it, inserted] = components.emplace(e, pool.allocate());
        if (!inserted) {
            *it->second = component;
        } else {
            std::construct_at(it->second, component);
        }
        return it->second;
    }

    void remove(VoxolEntity e) {
        auto it = components.find(e);
        if (it != components.end()) {
            pool.deallocate(it->second);
            components.erase(it);
        }
    }

    T* get(VoxolEntity e) {
        auto it = components.find(e);
        return it != components.end() ? it->second : nullptr;
    }

    const T* get(VoxolEntity e) const {
        auto it = components.find(e);
        return it != components.end() ? it->second : nullptr;
    }

    void clear() {
        for (auto& [e, ptr] : components) {
            pool.deallocate(ptr);
        }
        components.clear();
    }

private:
    std::unordered_map<VoxolEntity, T*> components;
    SlabPool<T> pool;
};

} // namespace Vox