#ifndef VOXOL_BASE_COMPONENT_STORAGE_H
#define VOXOL_BASE_COMPONENT_STORAGE_H

#include "SlabArena.h"
#include <unordered_map>
#include <type_traits>

namespace Voxol::Base
{
struct RemovableStorage
{
    virtual void removeEntity(VoxolEntity e) = 0;
    virtual ~RemovableStorage()              = default;
};
template <typename T>
class DefaultAllocPolicy
{
public:
    explicit DefaultAllocPolicy() = default;
    template <typename... Args>
    T* allocate(Args&&...)
    {
        return static_cast<T*>(operator new(sizeof(T)));
    }
    void deallocate(T* ptr)
    {
        operator delete(ptr);
    }
};

// component saving base class, has AllocPolicy
template <typename T, typename AllocPolicy = DefaultAllocPolicy<T>>
class BaseComponentStorage : public RemovableStorage
{
public:
    explicit BaseComponentStorage(AllocPolicy alloc = AllocPolicy()) noexcept
        :
        allocator(std::move(alloc))
    {
        printf("BaseComponentStorage::BaseComponentStorage() ...\n");
    }

    ~BaseComponentStorage()
    {
        printf("BaseComponentStorage::~BaseComponentStorage() ...\n");
        clear();
    }

    void removeEntity(VoxolEntity e) override
    {
        remove(e);
    }

    // copyable construction
    T* add(VoxolEntity e, const T& value)
    {
        auto [it, inserted] = components.emplace(e, nullptr);
        if (!inserted) return it->second;

        T* ptr = allocator.allocate();
        try
        {
            std::construct_at(ptr, value);
        }
        catch (...)
        {
            allocator.deallocate(ptr);
            components.erase(it);
            throw;
        }
        it->second = ptr;
        return ptr;
    }

    // Universal References

    // Forwarding References
    template <typename... Args>
    T* add(VoxolEntity e, Args&&... args)
    {
        auto [it, inserted] = components.emplace(e, nullptr);
        if (!inserted) return it->second;

        T* ptr = allocator.allocate(std::forward<Args>(args)...);
        try
        {
            // ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
            std::construct_at(ptr, std::forward<Args>(args)...);
        }
        catch (...)
        {
            allocator.deallocate(ptr);
            components.erase(it);
            throw;
        }
        it->second = ptr;
        return ptr;
    }

    void remove(VoxolEntity e)
    {
        auto it = components.find(e);
        if (it != components.end())
        {
            printf("BaseComponentStorage::remove() e: %d\n", e);
            std::destroy_at(it->second);
            allocator.deallocate(it->second);
            components.erase(it);
        }
    }

    T* get(VoxolEntity e)
    {
        auto it = components.find(e);
        return it != components.end() ? it->second : nullptr;
    }

    const T* get(VoxolEntity e) const
    {
        auto it = components.find(e);
        return it != components.end() ? it->second : nullptr;
    }

    bool has(VoxolEntity e) const
    {
        return components.find(e) != components.end();
    }

    const std::unordered_map<VoxolEntity, T*>& raw() const
    {
        return components;
    }

    void clear()
    {
        for (auto& [_, ptr] : components)
        {
            std::destroy_at(ptr);
            allocator.deallocate(ptr);
        }
        components.clear();
    }

private:
    std::unordered_map<VoxolEntity, T*> components;
    AllocPolicy                         allocator;
};

} // namespace Voxol::Base
#endif  // VOXOL_BASE_COMPONENT_STORAGE_H