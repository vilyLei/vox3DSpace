#ifndef VOXOL_COMPONENT_FACTORY_H
#define VOXOL_COMPONENT_FACTORY_H

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <utility>
#include <cstdio>

#include "BaseComponentStorage.h"

namespace Voxol::Base
{

class ComponentFactory : public std::enable_shared_from_this<ComponentFactory>
{
public:
    explicit ComponentFactory(SlabArena& arena) :
        arena(arena)
    {
        printf("ComponentFactory::ComponentFactory() ..\n");
    }
    ~ComponentFactory()
    {
        printf("ComponentFactory::~ComponentFactory() ..\n");
    }
    // 添加组件（完美转发构造）
    template <typename T, typename... Args>
    T* addComponent(VoxolEntity e, Args&&... args)
    {
        auto& storage = getOrCreateStorage<T>();
        return storage.add(e, std::forward<Args>(args)...);
    }

    // 添加组件（拷贝结构）
    template <typename T>
    T* addComponent(VoxolEntity e, const T& value)
    {
        auto& storage = getOrCreateStorage<T>();
        return storage.add(e, value);
    }

    // 替换组件（已有的删除再添加）
    template <typename T>
    T* replaceComponent(VoxolEntity e, const T& value)
    {
        removeComponent<T>(e);
        return addComponent<T>(e, value);
    }

    // 获取组件
    template <typename T>
    T* getComponent(VoxolEntity e)
    {
        auto it = storages.find(std::type_index(typeid(T)));
        if (it == storages.end()) return nullptr;
        return static_cast<Storage<T>*>(it->second.ptr)->get(e);
    }

    // 安全获取（可为空）
    template <typename T>
    T* tryGetComponent(VoxolEntity e)
    {
        return getComponent<T>(e);
    }

    template <typename T>
    const T* getComponent(VoxolEntity e) const
    {
        auto it = storages.find(std::type_index(typeid(T)));
        if (it == storages.end()) return nullptr;
        return static_cast<const Storage<T>*>(it->second.ptr)->get(e);
    }

    // 判断是否存在组件
    template <typename T>
    bool hasComponent(VoxolEntity e) const
    {
        auto it = storages.find(std::type_index(typeid(T)));
        if (it == storages.end()) return false;
        return static_cast<const Storage<T>*>(it->second.ptr)->has(e);
    }

    // 移除组件
    template <typename T>
    void removeComponent(VoxolEntity e)
    {
        auto it = storages.find(std::type_index(typeid(T)));
        if (it != storages.end())
        {
            static_cast<Storage<T>*>(it->second.ptr)->remove(e);
        }
    }

    // 遍历某类组件
    template <typename T, typename Func>
    void each(Func&& func)
    {
        auto it = storages.find(std::type_index(typeid(T)));
        if (it == storages.end()) return;
        auto* storage = static_cast<Storage<T>*>(it->second.ptr);
        for (const auto& [e, comp] : storage->raw())
        {
            func(e, *comp);
        }
    }

    // 删除指定实体的所有组件
    void removeAllComponents(VoxolEntity e)
    {
        for (auto& [_, wrapper] : storages)
        {
            if (wrapper.iface)
            {
                wrapper.iface->removeEntity(e);
            }
        }
    }

    // 清空所有组件
    void clearAllComponents()
    {
        for (auto& [_, wrapper] : storages)
        {
            wrapper.reset();
        }
        storages.clear();
    }

    // 工厂构造
    template <typename T = ComponentFactory>
    static std::shared_ptr<T> make(SlabArena& arena)
    {
        return std::shared_ptr<T>(new T(arena));
    }

private:
    SlabArena& arena;

    // 使用模板类型进行构造
    template <typename T>
    using Storage = BaseComponentStorage<T, SlabPool<T>>;

    struct StorageWrapper
    {

        void* ptr = nullptr;

        RemovableStorage*          iface = nullptr;
        std::function<void(void*)> deleter;

        StorageWrapper() = default;

        StorageWrapper(void* rawPtr, RemovableStorage* ifacePtr, std::function<void(void*)> del) :
            ptr(rawPtr), iface(ifacePtr), deleter(std::move(del)) {}

        // 移动构造
        StorageWrapper(StorageWrapper&& other) noexcept
            :
            ptr(other.ptr), iface(other.iface), deleter(std::move(other.deleter))
        {
            other.ptr   = nullptr;
            other.iface = nullptr;
            other.deleter = nullptr;
        }

        // 移动赋值
        StorageWrapper& operator=(StorageWrapper&& other) noexcept
        {
            if (this == &other)
                return *this;
            ptr     = other.ptr;
            iface   = other.iface;
            deleter = std::move(other.deleter);

            other.ptr   = nullptr;
            other.iface = nullptr;
            other.deleter = nullptr;
            
            return *this;
        }

        // 禁用拷贝
        StorageWrapper(const StorageWrapper&)            = delete;
        StorageWrapper& operator=(const StorageWrapper&) = delete;

        // 析构安全释放
        ~StorageWrapper()
        {
            reset();
        }

        // 提供手动释放接口
        void reset()
        {
            printf("StorageWrapper::reset() ...ptr: %zd\n", (size_t)ptr);
            if (!ptr) return;
            if (deleter) deleter(ptr);
            ptr   = nullptr;
            iface = nullptr;
            deleter = nullptr;
        }
    };

    // 创建组件存储结构
    template <typename T>
    Storage<T>& getOrCreateStorage()
    {
        auto key = std::type_index(typeid(T));
        auto it  = storages.find(key);
        if (it == storages.end())
        {
            auto* storage = new Storage<T>(SlabPool<T>(arena));
            storages[key] = StorageWrapper{
                storage,
                static_cast<RemovableStorage*>(storage),
                [](void* p) {
                    delete static_cast<Storage<T>*>(p);
                }};
            return *storage;
        }
        return *static_cast<Storage<T>*>(it->second.ptr);
    }

    std::unordered_map<std::type_index, StorageWrapper> storages;
};

} // namespace Voxol::Base

#endif // VOXOL_COMPONENT_FACTORY_H