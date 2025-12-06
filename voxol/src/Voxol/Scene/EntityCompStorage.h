#ifndef VOXOL_SCENE_ENTITY_COMP_STORAGE_H
#define VOXOL_SCENE_ENTITY_COMP_STORAGE_H

#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "../Math/Vec2.h"
#include "../Math/Mat33.h"

#include "CompPool.h"
#include "EntityComponent.h"
#include "EntityHistoryManager.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace Voxol::Scene
{

using EntityBoundsResponseCallType = std::function<void(const Base::ID::KeyUint64& etId, const Math::Bounds& bounds)>;

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
    uint32_t                 createEntity(uint32_t color, const Math::Vec2& size);
    uint32_t                 copyAndppendEntityFromId(uint32_t id);
    bool                     hasParentAt(uint32_t id) const;
    bool                     hasChildAt(uint32_t id) const;
    Component::UnitTransform getEntityLocalTransAt(uint32_t id);
    Component::UnitTransform getEntityParentLocalTransAt(uint32_t id);
    bool                     getEntityVisibleAt(uint32_t id);
    Math::Mat33              getEntityParentGlobalMatAt(uint32_t id);
    Math::Bounds             getEntityGlobalBoundsAt(uint32_t id);
    Math::Bounds             getEntityGlobalBoundsAt(const Base::ID::KeyUint64& id);
    Math::Bounds             getEntityLocalBoundsAt(uint32_t id);
    Math::Bounds             getEntityLocalBoundsAt(const Base::ID::KeyUint64& id);
    Math::Mat33              getEntityLocalMatrixAt(uint32_t id);
    Math::Mat33              getEntityLocalMatrixAt(const Base::ID::KeyUint64& id);

    uint32_t    getEntityParentIdAt(uint32_t id);
    Math::Vec2  getEntityGlobalXYAt(uint32_t id) const;
    void        setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id);
    Math::Mat33 getEntityWorldMatWithoutScale(uint32_t id);
    Math::Mat33 getEntityParentWorldMatWithoutScale(uint32_t id);
    Math::Vec2  getEntityLocalXYAt(uint32_t id) const;
    void        setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id);

    float                    getEntityRotationAt(uint32_t id) const;
    void                     setEntityRotationAt(float rad, uint32_t id);
    void                     setEntityColorAt(const Colour::Component::Color& color, uint32_t id);
    Colour::Component::Color getEntityColorAt(uint32_t id) const;
    void                     setEntityVisibleAt(bool v, uint32_t id);
    bool                     getEntityVisibleAt(uint32_t id) const;
    void                     setEntityPivotAt(const Math::Vec2& pivot, uint32_t id);
    Math::Vec2               getEntityPivotAt(uint32_t id) const;

    void getIdsFromId(uint32_t etId, std::vector<Base::ID::KeyUint64>& ids);


    Component::UnitTransform getEntityTransformAt(uint32_t id);
    void                     setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id);

    Math::Mat33 getEntityGlobalMat33At(const Base::ID::KeyUint64 id);
    Math::Mat33 getEntityGlobalMat33At(uint32_t id);
    void        checkIds(std::vector<Base::ID::KeyUint64>& edis);

    void traverseSortWithShadowEffect(const Base::ID::KeyUint64& srcKey, uint32_t nodeId, uint32_t& index);

    void updateHierarchyInfo();
    void traverseSortIndexWithInstance(uint32_t iid, uint32_t prototypeId, uint32_t& index);
    void traverseSortIndex(uint32_t etId, uint32_t& index);
    void traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat);

    void buildTopoOrderFromRoots(const std::vector<uint32_t>& roots);
    void traverseBuildGlobalMatInstance(uint32_t iid, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap);
    void traverseBuildGlobalMatPrototypeUnderInstance(uint32_t iid, uint32_t prototypeRootId, const Math::Mat33& instanceParentMat);
    void traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat);
    void traverseBuildGlobalMat(uint32_t etId);

    void markSubtreeDirty(uint32_t rootId);
    void updateDirtySubtrees(const std::vector<uint32_t>& roots);

    void collectShadowEffect(const Base::ID::KeyUint64& srcKey, uint32_t protoId, std::vector<Base::ID::KeyUint64>& ids);
    void collectAllEntitiesWithInstance(const Base::ID::KeyUint64& etId, std::vector<Base::ID::KeyUint64>& ids);
    void collectAllEntities(const Base::ID::KeyUint64& etId, std::vector<Base::ID::KeyUint64>& ids);
    void updateAllInstanceGlobalMats(const Base::ID::KeyUint64& etId);
    void setPrototypeEntitiesDirty(uint32_t etId, bool dirty, uint16_t reserveSize = 128);
    void foreachBoundsWithEntityId(uint32_t eId, EntityBoundsResponseCallType callback);
    void propagateVisibility(uint32_t rootId, bool visible = true);

public:
    uint32_t                               entityIdMax = 0;
    CompPool<Component::UnitEntity>        entitiesPool{};
    CompPool<Component::UnitShadingEntity> shaderingEntitiesPool{};
    CompPool<Component::UnitModel>         modelsPool{};
    CompPool<Component::UnitTransform>     transformsPool{};
    CompPool<Component::UnitHierarchy>     hierarchiesPool{};
    CompPool<Component::UnitShadingDesc>   shaderingDescPool{};

    Base::ID::keyUint64Unordered_map<uint32_t> hierarchyIndexMap{};

    std::unordered_map<uint32_t, Component::UnitEffectShadowDesc> effectShadowMap;
    Base::ID::keyUint64Unordered_map<Component::UnitShadowEntity> effectShadowEntityMap;
    // key: shadingId, value: effect ids
    std::unordered_map<uint32_t, std::vector<uint32_t>> shadingShadowIdMap;

    std::unordered_map<uint32_t, Math::Mat33>                entityGlobalMat33Map;
    Base::ID::keyUint64Unordered_map<Math::Mat33>            entityInsGlobalMat33Map;
    std::unordered_map<uint32_t, Component::UnitInstanceMap> instanceStorage;
    std::unordered_map<uint32_t, Component::UnitTextModel>   entityStringModelMap;


    std::unordered_map<uint32_t, Component::UnitIdName>         interactionIDMap;
    std::unordered_map<uint32_t, Component::UnitIdName>         actionIDMap;
    std::unordered_map<std::string, Component::UnitIdName>      entityNameMap;
    std::unordered_map<uint32_t, Scene::Component::UnitTexture> textureMap;
    std::unordered_map<uint32_t, Math::Vec2>                    transformPivotMap;
    std::unordered_map<uint32_t, bool>                          dirtyIdMap;

private:
    std::vector<uint32_t> topoOrder;
    std::vector<uint32_t> topoIndex;
};
} // namespace Voxol::Scene
#endif
