#include "EntityRenderSystem.h"

namespace Voxol::Render
{

void EntityRenderSystem::initalize()
{
    auto total = 10;
    entities.resize(total);
    shaderingEntities.resize(total);
    boundsVec.resize(total);

    shaderingDescVec.resize(total * 2);


    for (auto i = 0; i < entities.size(); ++i)
    {
        entities[i].id = i;
    }
    for (auto i = 0; i < shaderingEntities.size(); ++i)
    {
        shaderingEntities[i].id = i;
    }
    for (auto i = 0; i < boundsVec.size(); ++i)
    {
        boundsVec[i].id = i;
    }

    for (auto i = 0; i < shaderingDescVec.size(); ++i)
    {
        shaderingDescVec[i].id = i;
    }


    boundsVec[0].bounds.setXYWH(150, 50, 200, 200);
    boundsVec[1].bounds.setXYWH(510, 150, 100, 100);
    boundsVec[2].bounds.setXYWH(250, 50, 150, 150);

    shaderingDescVec[0].color     = 0xff880077;
    shaderingDescVec[0].transform = {150, 50, 200, 200, 0};
    shaderingDescVec[1].color     = 0xff008855;
    shaderingDescVec[1].transform = {150, 50, 200, 200, 0};

    shaderingDescVec[2].color     = 0xff002233;
    shaderingDescVec[2].transform = {510, 150, 100, 100, 0};

    shaderingDescVec[3].color     = 0xff006666;
    shaderingDescVec[3].transform = {250, 50, 150, 150, 0};

    /// circle
    shaderingEntities[0].drawUnitId    = 0;
    shaderingEntities[0].shadingDescId = 0;

    /// circle
    shaderingEntities[1].drawUnitId    = 0;
    shaderingEntities[1].shadingDescId = 1;

    /// circle
    shaderingEntities[2].drawUnitId    = 0;
    shaderingEntities[2].shadingDescId = 2;

    /// multi-circles
    shaderingEntities[3].drawUnitId    = 2;
    shaderingEntities[3].shadingDescId = 3;

    /// circle
    //entities[0].boundsId  = 0;
    entities[0].shadingId = 0;

    /// circle
    //entities[1].boundsId  = 0;
    entities[1].shadingId = 1;

    /// circle
    //entities[2].boundsId  = 1;
    entities[2].shadingId = 2;

    /// multi-circles
    //entities[3].boundsId  = 2;
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

        //auto& b = boundsVec[et.boundsId].bounds;
        auto& shadingEt = shaderingEntities[et.shadingId];
        auto& shdDesc   = shaderingDescVec[shadingEt.shadingDescId];
        auto& trans     = shdDesc.transform;

        //b.setXYWH(trans.x, trans.y, trans.sx, trans.sy);

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
    //auto total = entities.size();
    //for (auto i = 0; i < total; i++)
    //{
    //    auto& et = entities[i];
    //    if (et.boundsId < 0 || !et.visible)
    //        continue;
    //    auto& b = boundsVec[et.boundsId].bounds;
    //    if (wbounds.intersects(b))
    //    {
    //        queriedEIds.push_back(et.id);
    //    }
    //}
    return static_cast<int>(queriedEIds.size());
}
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{
    auto tot = queriedEIds.size();
    for (auto i = 0; i < tot; i++)
    {
        auto& item = bvhItems[queriedEIds[i]];
        auto& et   = entities[item.objectId];
        if (et.boundsId < 0 || et.shadingId < 0 || !et.visible)
            continue;
        drawUnit(et, rctx, vpM, drawingUnits);
    }
}

void EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{
    auto& shadingEt = shaderingEntities[entity.shadingId];
    auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto& shdDesc   = shaderingDescVec[shadingEt.shadingDescId];
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