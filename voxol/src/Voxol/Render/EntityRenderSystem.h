#ifndef VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H
#define VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H

#include "../Tile/GridDef.h"
#include "BVH2DV1.h"
#include "DrawCtx.h"
#include "CompPool.h"
#include "EntityComponent.h"
#include "EntityCompStorage.h"
#include "DrawingUnitStorage.h"

namespace Voxol::Render
{

//using namespace Voxol::Tile;

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
    int  drawQuery(const Math::VxRect& bounds, int phase);
    void render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds);
    void clear();

    V1::BVH2D::SP          bvh = V1::BVH2D::make();
    EntityCompStorage::SP compStorage{};
    DrawingUnitStorage::SP drawingStorage{};

private:
    bool                 drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds);
    std::vector<int32_t> queriedEIds{};
};
} // namespace Voxol::Render
#endif
