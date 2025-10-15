#include "EntityRenderSystem.h"

namespace Voxol::Render
{

void EntityRenderSystem::initalize()
{
    auto total = 10;

    auto& entities = storage.entities;
    auto& shaderingEntitiesPool = storage.shaderingEntitiesPool;
    auto& shaderingDescPool     = storage.shaderingDescPool;

    entities.resize(total);
    //shaderingEntities.resize(total);
    shaderingEntitiesPool.initialize(total);

    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);

    for (auto i = 0; i < entities.size(); ++i)
    {
        entities[i].id = i;
    }
    //for (auto i = 0; i < shaderingEntities.size(); ++i)
    for (auto i = 0; i < total; ++i)
    {
        shaderingEntitiesPool[i].id = i;
    }

    for (auto i = 0; i < shaderingDescTotal; ++i)
    {
        shaderingDescPool[i].id = i;
    }

    shaderingDescPool[0].color    = 0xff880077;
    shaderingDescPool[0].transform = {150, 50, 200, 200, 0};
    shaderingDescPool[1].color     = 0xff008855;
    shaderingDescPool[1].transform = {150, 50, 200, 200, 0};

    shaderingDescPool[2].color    = 0xff002233;
    shaderingDescPool[2].transform = {510, 150, 100, 100, 0};

    shaderingDescPool[3].color    = 0xff660066;
    shaderingDescPool[3].transform = {250, 50, 150, 150, 0};

    /// circle
    shaderingEntitiesPool[0].drawUnitId    = 0;
    shaderingEntitiesPool[0].shadingDescId = 0;

    /// circle
    shaderingEntitiesPool[1].drawUnitId = 0;
    shaderingEntitiesPool[1].shadingDescId = 1;

    /// circle
    shaderingEntitiesPool[2].drawUnitId = 0;
    shaderingEntitiesPool[2].shadingDescId = 2;

    /// multi-circles
    shaderingEntitiesPool[3].drawUnitId = 2;
    shaderingEntitiesPool[3].shadingDescId = 3;

    /// circle
    entities[0].shadingId = 0;

    /// circle
    entities[1].shadingId = 1;

    /// circle
    entities[2].shadingId = 2;

    /// multi-circles
    entities[3].shadingId = 3;

    // update bounds
    auto boundsTotal = 0;
    for (auto& et : entities)
    {
        if (et.shadingId < 0)
        {
            continue;
        }
        boundsTotal++;
    }

    bvhItems.resize(boundsTotal);
    auto boundsIndex = 0;

    std::unordered_map<uint32_t, uint32_t> map{};
    for (auto& et : entities)
    {
        if (et.shadingId < 0 || map.contains(et.boundsId))
        {
            continue;
        }
        map[boundsIndex] = 1;

        auto& shadingEt = shaderingEntitiesPool[et.shadingId];
        auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
        auto& trans     = shdDesc.transform;

        auto& b = bvhItems[boundsIndex];
        b.bounds.setXYWH(trans.x, trans.y, trans.sx, trans.sy);
        b.objectId = et.id;
        et.boundsId = boundsIndex;
        boundsIndex++;
    }
    bvh.build(bvhItems);
}

int EntityRenderSystem::drawQuery(const Math::VxRect& wbounds, const Math::Mat33& vpM)
{
    queriedEIds.clear();
    bvh.query(wbounds, queriedEIds);
    return static_cast<int>(queriedEIds.size());
}
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{

    auto& entities = storage.entities;
    auto tot = queriedEIds.size();
    for (auto i = 0; i < tot; i++)
    {
        auto& item = bvhItems[queriedEIds[i]];
        auto& et   = entities[item.objectId];
        if (et.shadingId < 0 || !et.visible)
            continue;
        drawUnit(et, rctx, vpM, drawingUnits);
    }
}

void EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{

    auto& shaderingDescPool = storage.shaderingDescPool;

    //auto& shadingEt = shaderingEntitiesPool[entity.shadingId];
    auto& shadingEt = storage.getCompAt<Component::UnitShadingEntity>(entity.shadingId);
    auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
    auto& trans     = shdDesc.transform;

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdDesc.color);
    drawUnit.objMat.setXY(trans.x, trans.y);
    drawUnit.objMat.setScaleXY(trans.sx, trans.sy);
    drawUnit.mvp = vpM;
    drawUnit.draw();
}
void EntityRenderSystem::clear()
{
}
} // namespace Voxol::Render