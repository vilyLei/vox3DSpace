#ifndef VOXOL_RENDER_ENTITY_COMP_STORAGE_H
#define VOXOL_RENDER_ENTITY_COMP_STORAGE_H

#include "../Tile/GridDef.h"
#include "CompPool.h"
#include "EntityComponent.h"
#include <vector>

namespace Voxol::Render
{

class EntityCompStorage
{
public:
    EntityCompStorage()   = default;
    ~EntityCompStorage() = default;

public:
    template <typename T>
    CompPool<T>& getPool()
    {
        if constexpr (std::is_same_v<T, Component::UnitShadingEntity>)
            return shaderingEntitiesPool;
        else if constexpr (std::is_same_v<T, Component::UnitShadingBaseDesc>)
            return shaderingDescPool;
        else
            static_assert(!sizeof(T), "Unsupported pool type");
    }
    template <typename T>
    const CompPool<T>& getPool() const
    {
        if constexpr (std::is_same_v<T, Component::UnitShadingEntity>)
            return shaderingEntitiesPool;
        else if constexpr (std::is_same_v<T, Component::UnitShadingBaseDesc>)
            return shaderingDescPool;
        else
            static_assert(!sizeof(T), "Unsupported pool type");
    }

    template <typename T>
    const T& getCompAt(int32_t index) const
    {
        const auto& pool = getPool<T>();
        return pool.get(index);
    }
    template <typename T>
    T& get(int32_t index)
    {
        return getPool<T>().get(index);
    }

    template <typename T>
    const T& get(int32_t index) const
    {
        return getPool<T>().get(index);
    }
    //template <typename T>
    //const T& operator[](int32_t index) const
    //{
    //    const auto& pool = getPool<T>();
    //    return pool.get(index);
    //}
    //template <typename T>
    //T& operator[](int32_t index) const
    //{
    //    auto& pool = getPool<T>();
    //    return pool.get(index);
    //}

public:
    std::vector<Component::UnitEntity>        entities{};
    CompPool<Component::UnitShadingEntity>     shaderingEntitiesPool{};
    CompPool<Component::UnitShadingBaseDesc>  shaderingDescPool{};
};
} // namespace Voxol::Render
#endif
