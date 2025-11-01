#ifndef VOXOL_RENDER_ENTITY_COMP_STORAGE_H
#define VOXOL_RENDER_ENTITY_COMP_STORAGE_H

//#include "../Tile/GridDef.h"

#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "../Math/Vec2.h"
#include "../Math/Mat33.h"

#include "CompPool.h"
#include "EntityComponent.h"
#include "EntityHistoryManager.h"
#include <vector>
#include <unordered_map>

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

    bool                     hasParentAt(uint32_t id) const;
    bool                     hasChildAt(uint32_t id) const;
    Component::UnitTransform getEntityLocalTransAt(uint32_t id);
    Component::UnitTransform getEntityParentLocalTransAt(uint32_t id);
    bool                     getEntityVisibleAt(uint32_t id);
    Math::Mat33              getEntityParentGlobalMatAt(uint32_t id);
    Math::Mat33              getEntityGlobalMatAt(uint32_t id);
    Math::Bounds             getEntityGlobalBoundsAt(uint32_t id);
    uint32_t                 getEntityParentIdAt(uint32_t id);
    Math::Vec2               getEntityGlobalXYAt(uint32_t id);
    void                     setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id);
    Math::Mat33              getEntityWorldMatWithoutScale(uint32_t id);
    Math::Mat33              getEntityParentWorldMatWithoutScale(uint32_t id);
    Math::Vec2               getEntityLocalXYAt(uint32_t id);
    void                     setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id);

    void getIdsFromId(uint32_t etId, std::vector<uint32_t>& ids);


    Component::UnitTransform getEntityTransformAt(uint32_t id);
    void                     setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id);

    Math::Mat33 getEntityGlobalMat33At(uint32_t id);
    void        checkIds(std::vector<uint32_t>& edis);

    void updateHierarchyInfo();
    void traverseSortIndex(uint32_t etId, uint32_t& index);
    void traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat);
    void traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat);

    void buildTopoOrderFromRoots(const std::vector<uint32_t>& roots);
    void traverseBuildWorldMatInstance(uint32_t instanceRootId, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap);
    void traverseBuildWorldMatPrototypeUnderInstance(uint32_t instanceEntityId, uint32_t prototypeRootId, const Math::Mat33& instanceParentMat);

public:
    CompPool<Component::UnitEntity>           entitiesPool{};
    CompPool<Component::UnitShadingEntity>    shaderingEntitiesPool{};
    CompPool<Component::UnitModel>            modelsPool{};
    CompPool<Component::UnitTransform>        transformsPool{};
    CompPool<Component::UnitHierarchy>        hierarchiesPool{};
    CompPool<Component::UnitShadingBaseDesc>  shaderingDescPool{};
    CompPool<Component::UnitMat33>            unitWMat33Pool{};
    std::unordered_map<uint32_t, uint32_t>    hierarchyIndexMap{};
    std::unordered_map<uint32_t, Math::Mat33> entityGlobalMat33Map{};
    // for debug
    std::unordered_map<Component::EntityId, Math::Mat33, Component::EntityIdHasher, Component::EntityIdEqual> entityIdTypeMatMap{};

private:
    std::vector<uint32_t> topoOrder;
    std::vector<uint32_t> topoIndex;
    std::unordered_map<uint32_t, Component::UnitInstanceMap> insStorage;
};
} // namespace Voxol::Render
#endif
