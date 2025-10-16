#ifndef VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H
#define VOXOL_RENDER_ENTITY_RENDER_SYSTEM_H

#include "../Tile/GridDef.h"
//#include "BVH2D.h"
#include "BVH2DV1.h"
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
    void       initalize();
    Math::Vec2 getEntityXYAt(int32_t id);
    void       setEntityXYAt(const Math::Vec2& pos, int32_t id);
    int        drawQuery(const Math::VxRect& bounds, const Math::Mat33& vpM);
    void       render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    void       clear();

    //std::vector<BVHItem2D> bvhItems;
    std::vector<V1::BVH2D::Item> bvhItems;
    V1::BVH2D                    bvh{};

private:
    void                 drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    EntityCompStorage    storage{};
    std::vector<int32_t> queriedEIds{};
};
} // namespace Voxol::Render
#endif
