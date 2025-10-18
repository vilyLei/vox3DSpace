#include "EntityRenderSystem.h"

namespace Voxol::Render
{

void EntityRenderSystem::initalize()
{
    if (storage)
    {
        return;
    }
    storage = EntityCompStorage::make();

    auto total = 10;

    auto& entities = storage->entities;
    auto& shaderingEntitiesPool = storage->shaderingEntitiesPool;
    auto& shaderingDescPool     = storage->shaderingDescPool;

    entities.resize(total);
    shaderingEntitiesPool.initialize(total);

    auto shaderingDescTotal = total * 2;
    shaderingDescPool.initialize(shaderingDescTotal);

    for (auto i = 0; i < entities.size(); ++i)
    {
        entities[i].id = i;
    }

    shaderingEntitiesPool.forEach([&](auto& e, int32_t index) {
        e.id = index;
    });

    shaderingDescPool[0].color    = 0xff880077;
    shaderingDescPool[0].transform = {150, 50, 50, 50, 0};
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
    ///// circle
    //entities[1].shadingId = 1;
    ///// circle
    //entities[2].shadingId = 2;
    ///// multi-circles
    //entities[3].shadingId = 3;


    Math::Bounds                           bounds{};
    std::unordered_map<uint32_t, uint32_t> map{};
    for (auto& et : entities)
    {
        if (et.shadingId < 0)
        {
            continue;
        }

        auto& shadingEt = shaderingEntitiesPool[et.shadingId];
        auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
        auto& trans     = shdDesc.transform;

        bounds.setXYWH(trans.x, trans.y, trans.sx, trans.sy);

        bvh.addItem(et.id, bounds);
    }
    bvh.build();
}

int EntityRenderSystem::drawQuery(const Math::VxRect& wbounds, int phase)
{
    queriedEIds.clear();
    bvh.queryBounds(wbounds, queriedEIds);
    if (phase < 2)
        printf("EntityRenderSystem::drawQuery() A size: %d, phase: %d, bounds total: %d\n", queriedEIds.size(), phase, bvh.getBoundsCapacity());

    /// for test
    auto flag = false;
    if (queriedEIds.empty())
    {
        flag = true;
    }
    return static_cast<int>(queriedEIds.size());
}
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{
    if (!storage)
        return;

    //printf("EntityRenderSystem::render() B %d\n", queriedEIds.size());
    /// 这里是正确的写法
    if (queriedEIds.empty())
        return;

    auto  total    = queriedEIds.size();
    auto& entities = storage->entities;
    for (auto i = 0; i < total; i++)
    {
        auto& et = entities[queriedEIds[i]];
        if (et.shadingId < 0 || !et.visible)
            continue;
        drawUnit(et, rctx, vpM, drawingUnits);
    }
    return;

    // 暂时这样写，以便测试dragging
    auto& ets = storage->entities;
    auto  tot = ets.size();
    for (auto i = 0; i < tot; i++)
    {
        auto& et = ets[i];
        if (et.shadingId < 0 || !et.visible)
            continue;
        drawUnit(et, rctx, vpM, drawingUnits);
    }
}

void EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Draw::DrawContext& rctx, const Math::Mat33& vpM, std::vector<Gpu::DrawingUnit> drawingUnits)
{
    if (!storage)
        return;

    auto& shaderingDescPool = storage->shaderingDescPool;

    const auto& shadingEt = storage->get<Component::UnitShadingEntity>(entity.shadingId);
    auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
    auto& trans     = shdDesc.transform;
    //printf("trans(x=%f, y=%f)\n", trans.x, trans.y);
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