#ifndef VOXOL_RENDER_ENTITY_COMP_STORAGE_H
#define VOXOL_RENDER_ENTITY_COMP_STORAGE_H

#include "../Tile/GridDef.h"
#include "CompPool.h"
#include "EntityComponent.h"
#include "EntityHistoryManager.h"
#include <vector>

namespace Voxol::Render
{

class EntityCompStorage
{
public:
    using SP = std::shared_ptr<EntityCompStorage>;
    using WP = std::weak_ptr<EntityCompStorage>;
    using UP = std::unique_ptr<EntityCompStorage>;

public:
    static EntityCompStorage::SP make();

public:
    EntityCompStorage()  = default;
    ~EntityCompStorage() = default;

public:


    EntityHistoryManager::SP historyManager = EntityHistoryManager::make();

    template <typename T>
    CompPool<T>& getPool()
    {
        if constexpr (std::is_same_v<T, Component::UnitShadingEntity>)
            return shaderingEntitiesPool;
        else if constexpr (std::is_same_v<T, Component::UnitShadingBaseDesc>)
            return shaderingDescPool;
        else if constexpr (std::is_same_v<T, Component::UnitTransform>)
            return transformsPool;
        else if constexpr (std::is_same_v<T, Component::UnitModel>)
            return modelsPool;
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
        else if constexpr (std::is_same_v<T, Component::UnitTransform>)
            return transformsPool;
        else if constexpr (std::is_same_v<T, Component::UnitModel>)
            return modelsPool;
        else
            static_assert(!sizeof(T), "Unsupported pool type");
    }

    template <typename T>
    const T& getCompAt(uint32_t index) const
    {
        const auto& pool = getPool<T>();
        return pool.get(index);
    }
    template <typename T>
    T& get(uint32_t index)
    {
        return getPool<T>().get(index);
    }

    template <typename T>
    const T& get(uint32_t index) const
    {
        return getPool<T>().get(index);
    }
    //template <typename T>
    //const T& operator[](uint32_t index) const
    //{
    //    const auto& pool = getPool<T>();
    //    return pool.get(index);
    //}
    //template <typename T>
    //T& operator[](uint32_t index) const
    //{
    //    auto& pool = getPool<T>();
    //    return pool.get(index);
    //}

    Component::UnitTransform getEntityTransformAt(uint32_t id);
    void                     setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id);

    Math::Vec2 getEntityXYAt(uint32_t id);
    void       setEntityXYAt(const Math::Vec2& pos, uint32_t id);
    void       checkIds(std::vector<uint32_t>& edis);

public:

    CompPool<Component::UnitEntity>          entitiesPool{};
    CompPool<Component::UnitShadingEntity>   shaderingEntitiesPool{};
    CompPool<Component::UnitModel>       modelsPool{};
    CompPool<Component::UnitTransform>       transformsPool{};
    CompPool<Component::UnitHierarchy>       hierarchiesPool{};
    CompPool<Component::UnitShadingBaseDesc> shaderingDescPool{};
    CompPool<Component::UnitMat33>           unitWMat33Pool{};
};
} // namespace Voxol::Render
#endif
