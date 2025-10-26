#ifndef VOXOL_RENDER_ENTITY_SCENE_SYSTEM_H
#define VOXOL_RENDER_ENTITY_SCENE_SYSTEM_H

#include "BVH2DV1.h"
#include "DrawCtx.h"

#include "EntityUnitStorage.h"

namespace Voxol::Render
{

using EntitySysBVH = V1::BVH2D;

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
    void                       initalize(const std::string& configFileName = {});
    int                        drawQuery(const Math::VxRect& bounds, int phase);
    void                       clear();
    const std::vector<int32_t> getQueriedEIds() const;

    EntitySysBVH::SP      bvh = EntitySysBVH::make();
    EntityUnitStorage::SP entityStorage{};
    //EntityUnitStorage::SP entityStorage01{};

private:
    std::vector<int32_t> queriedEIds{};
};
} // namespace Voxol::Render
#endif
