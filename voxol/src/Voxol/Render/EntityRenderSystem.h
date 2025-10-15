#ifndef VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H
#define VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H

#include "../Tile/GridDef.h"
#include "BVH2D.h"
#include "DrawCtx.h"
#include "CompPool.h"
#include "EntityComponent.h"
#include "EntityCompStorage.h"

namespace Voxol::Render
{

using namespace Voxol::Test;
using namespace Voxol::Tile;

class EntityRenderSystem
{
public:
    EntityRenderSystem()  = default;
    ~EntityRenderSystem() = default;

public:
    void initalize();
    int  drawQuery(const Math::VxRect& bounds, const Math::Mat33& vpM);
    void render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    void clear();

    std::vector<BVHItem2D> bvhItems;
    BVH2D                  bvh{};

private:
    void                 drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    EntityCompStorage    storage{};
    std::vector<int32_t> queriedEIds{};
};
} // namespace Voxol::Render
#endif
