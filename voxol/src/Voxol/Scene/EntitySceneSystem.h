#ifndef VOXOL_SCENE_ENTITY_SCENE_SYSTEM_H
#define VOXOL_SCENE_ENTITY_SCENE_SYSTEM_H

#include "BVH2DV2.h"
#include "DrawCtx.h"

#include "EntityUnitStorage.h"
#include "EntityMotionObjectStorage.h"
#include "../Intent/InteractionSourceSystem.h"
#include <functional>

namespace Voxol::Scene
{
using EntitySysBVH = V2::BVH2D;

class EntitySceneSystem
{
public:
    using SP = std::shared_ptr<EntitySceneSystem>;
    using WP = std::weak_ptr<EntitySceneSystem>;
    using UP = std::unique_ptr<EntitySceneSystem>;

public:
    static EntitySceneSystem::SP make();

public:
    EntitySceneSystem()  = default;
    ~EntitySceneSystem() = default;

public:
    Intent::InteractionSourceSystem::SP interSrcSys;
    Intent::ActionSystem::SP             actionSys;
    Scene::EntityMotionObjectStorage::SP motionObjStorage;
    EntitySysBVH::SP                    bvh = EntitySysBVH::make();
    EntityUnitStorage::SP               entityStorage{};

public:
    void                             initalize(const std::string& configFileName = {});
    void                             createEntities(int total);
    void                             update();
    int                              drawQuery(const Math::VxRect& bounds, int phase);
    void                             clear();
    const std::vector<Base::ID::KeyUint64> getQueriedEIds() const;
    void                             updateBoundsWithEntityId(uint32_t eId, EntityBoundsResponseCallType callback);

private:
    std::vector<Base::ID::KeyUint64> queriedEIds{};
};
} // namespace Voxol::Scene
#endif
