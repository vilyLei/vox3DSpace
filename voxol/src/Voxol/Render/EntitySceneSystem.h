#ifndef VOXOL_RENDER_ENTITY_SCENE_SYSTEM_H
#define VOXOL_RENDER_ENTITY_SCENE_SYSTEM_H

//#include "BVH2DV1.h"
#include "BVH2DV2.h"
#include "DrawCtx.h"

#include "EntityUnitStorage.h"
#include <functional>

namespace Voxol::Render
{

//using EntitySysBVH = V1::BVH2D;
using EntitySysBVH = V2::BVH2D;

using BoundsUpdateCallType = std::function<void(const Render::ID::KeyUint64& etId, const Math::Bounds& bounds)>;
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
    void                             initalize(const std::string& configFileName = {});
    int                              drawQuery(const Math::VxRect& bounds, int phase);
    void                             clear();
    const std::vector<ID::KeyUint64> getQueriedEIds() const;

    EntitySysBVH::SP      bvh = EntitySysBVH::make();
    EntityUnitStorage::SP entityStorage{};
    void                  updateBVHBoundsWithEntityId(uint32_t eId, BoundsUpdateCallType callback);

private:
    std::vector<ID::KeyUint64> queriedEIds{};
};
} // namespace Voxol::Render
#endif
