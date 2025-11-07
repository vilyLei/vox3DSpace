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
        else if constexpr (std::is_same_v<T, Component::UnitShadingDesc>)
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
        else if constexpr (std::is_same_v<T, Component::UnitShadingDesc>)
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
    //Math::Mat33              getEntityGlobalMatAt(uint32_t id);
    Math::Bounds getEntityGlobalBoundsAt(uint32_t id);
    uint32_t     getEntityParentIdAt(uint32_t id);
    Math::Vec2   getEntityGlobalXYAt(uint32_t id);
    void         setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id);
    Math::Mat33  getEntityWorldMatWithoutScale(uint32_t id);
    Math::Mat33  getEntityParentWorldMatWithoutScale(uint32_t id);
    Math::Vec2   getEntityLocalXYAt(uint32_t id);
    void         setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id);

    void getIdsFromId(uint32_t etId, std::vector<ID::KeyUint64>& ids);


    Component::UnitTransform getEntityTransformAt(uint32_t id);
    void                     setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id);

    Math::Mat33 getEntityGlobalMat33At(const ID::KeyUint64 id);
    Math::Mat33 getEntityGlobalMat33At(uint32_t id);
    void        checkIds(std::vector<ID::KeyUint64>& edis);

    void traverseSortWithShadowEffect(const ID::KeyUint64& srcKey, uint32_t nodeId, uint32_t& index);

    void updateHierarchyInfo();
    void traverseSortIndexWithInstance(uint32_t iid, uint32_t prototypeId, uint32_t& index);
    void traverseSortIndex(uint32_t etId, uint32_t& index);
    void traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat);

    void buildTopoOrderFromRoots(const std::vector<uint32_t>& roots);
    void traverseBuildGlobalMatInstance(uint32_t iid, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap);
    void traverseBuildGlobalMatPrototypeUnderInstance(uint32_t iid, uint32_t prototypeRootId, const Math::Mat33& instanceParentMat);
    void traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat);

    void markSubtreeDirty(uint32_t rootId);
    void updateDirtySubtrees(const std::vector<uint32_t>& roots);

    void collectShadowEffect(const ID::KeyUint64& srcKey, uint32_t protoId, std::vector<ID::KeyUint64>& ids);
    void collectAllEntitiesWithInstance(const ID::KeyUint64& etId, std::vector<ID::KeyUint64>& ids);
    void collectAllEntities(const ID::KeyUint64& etId, std::vector<ID::KeyUint64>& ids);
    void updateAllInstanceGlobalMats(const ID::KeyUint64& etId);
    void setPrototypeEntitiesDirty(uint32_t etId, bool dirty, uint16_t reserveSize = 128);

public:
    CompPool<Component::UnitEntity>        entitiesPool{};
    CompPool<Component::UnitShadingEntity> shaderingEntitiesPool{};
    CompPool<Component::UnitModel>         modelsPool{};
    CompPool<Component::UnitTransform>     transformsPool{};
    CompPool<Component::UnitHierarchy>     hierarchiesPool{};
    CompPool<Component::UnitShadingDesc>   shaderingDescPool{};

    ID::keyUint64Unordered_map<uint32_t> hierarchyIndexMap{};

    std::unordered_map<uint32_t, Component::UnitEffectShadowDesc> effectShadowMap{};
    ID::keyUint64Unordered_map<Component::UnitShadowEntity>       effectShadowEntityMap{};
    // key: shadingId, value: effect ids
    std::unordered_map<uint32_t, std::vector<uint32_t>> shadingShadowIdMap{};

    std::unordered_map<uint32_t, Math::Mat33>                entityGlobalMat33Map{};
    ID::keyUint64Unordered_map<Math::Mat33>                  entityInsGlobalMat33Map{};
    std::unordered_map<uint32_t, Component::UnitInstanceMap> instanceStorage;

private:
    std::vector<uint32_t> topoOrder;
    std::vector<uint32_t> topoIndex;
};
} // namespace Voxol::Render
#endif
