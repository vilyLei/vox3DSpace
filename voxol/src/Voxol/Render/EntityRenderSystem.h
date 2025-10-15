#ifndef VOXOL_ENTITY_RENDER_SYSTEM_H
#define VOXOL_ENTITY_RENDER_SYSTEM_H

#include "../Tile/GridDef.h"
#include "BVH2D.h"
#include "DrawCtx.h"
#include "CompPool.h"

namespace Voxol::Render
{

using namespace Voxol::Test;
using namespace Voxol::Tile;

namespace Component
{

//struct UnitBounds
//{
//    int32_t      id = -1;
//    Math::VxRect bounds{};
//    bool         dirty = true;
//};
struct UnitTransform
{
    float x = 0;
    float y = 0;

    /// x-axis scale
    float sx = 1;
    /// y-axis scale
    float sy = 1;

    ///  rotation radian
    float rotation = 0;
};

struct UnitShadingBaseDesc
{
    int32_t       id = -1;
    UnitTransform transform{};
    uint32_t      color = 0xff000000;
};

struct UnitShadingEntity
{
    int32_t id = -1;
    /// UnitShadingBaseDesc vector index value
    int32_t shadingDescId = -1;
    /// DrawingUnit vector index value
    int32_t drawUnitId = -1;
};

struct UnitEntity
{
    int32_t id = -1;
    /// UnitBounds vector index value
    int32_t boundsId = -1;
    /// UnitShadingEntity vector index value
    int32_t shadingId = -1;
    bool    visible   = true;
};

} // namespace Component

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
    void                                      drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits);
    std::vector<Component::UnitEntity>        entities{};
    //std::vector<Component::UnitShadingEntity> shaderingEntities{};
    CompPool<Component::UnitShadingEntity>     shaderingEntitiesPool{};
    CompPool<Component::UnitShadingBaseDesc>  shaderingDescPool{};
    std::vector<int32_t>                     queriedEIds{};
};
} // namespace Voxol::Render
#endif
