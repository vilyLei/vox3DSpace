#ifndef VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H
#define VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H

#include "BVH2DV1.h"
#include "DrawCtx.h"
#include "CompPool.h"

#include "EntityUnitStorage.h"

namespace Voxol::Render
{

using EntitySysBVH = V1::BVH2D;

class EntityRenderSystem
{
public:
    using SP = std::shared_ptr<EntityRenderSystem>;
    using WP = std::weak_ptr<EntityRenderSystem>;
    using UP = std::unique_ptr<EntityRenderSystem>;

public:
    static EntityRenderSystem::SP make();

public:
    EntityRenderSystem()  = default;
    ~EntityRenderSystem() = default;

public:
    void initalize();
    void render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds, const std::vector<ID::KeyUint64>& queriedEIds);
    void clear();

    EntityUnitStorage::SP entityStorage{};

private:
    bool drawSingleUnitEffect(const Math::Mat33& vpM, const ID::KeyUint64 etKey, const Math::Bounds& wbounds);
    bool drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds, const Math::Mat33& wM);
    //bool drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds);
};
} // namespace Voxol::Render
#endif
