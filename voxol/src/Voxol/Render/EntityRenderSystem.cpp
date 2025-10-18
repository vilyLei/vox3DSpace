#include "EntityRenderSystem.h"

namespace Voxol::Render
{

EntityRenderSystem::SP EntityRenderSystem::make()
{
    auto sp = std::make_shared<EntityRenderSystem>();
    return sp;
}

void EntityRenderSystem::initalize()
{
    if (compStorage)
    {
        return;
    }

    compStorage = EntityCompStorage::make();
    if (!drawingStorage) {
        drawingStorage = DrawingUnitStorage::make();
    }

    auto storage = compStorage;
    auto total = 512;
    drawingStorage->initalize( total );

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
    shaderingDescPool[0].transform = {150, 50, 200, 200, 0};
    shaderingDescPool[1].color     = 0xff008855;
    shaderingDescPool[1].transform = {150, 50, 200, 200, 0};

    shaderingDescPool[2].color    = 0xff554433;
    shaderingDescPool[2].transform = {510, 150, 100, 100, 0};

    shaderingDescPool[3].color    = 0xff660066;
    shaderingDescPool[3].transform = {250, 50, 150, 150, 0};

    shaderingDescPool[4].color     = 0xff00aa76;
    shaderingDescPool[4].transform = {600, 150, 200, 200, 0};

    shaderingDescPool[5].color     = 0xff00aa76;
    shaderingDescPool[5].transform = {509, 350, 200, 200, 0};

    /// circle
    shaderingEntitiesPool[0].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[0].shadingDescId = 0;

    /// circle
    shaderingEntitiesPool[1].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[1].shadingDescId = 1;

    /// circle
    shaderingEntitiesPool[2].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::Circle);
    shaderingEntitiesPool[2].shadingDescId = 2;

    /// ring
    shaderingEntitiesPool[3].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::Ring);
    shaderingEntitiesPool[3].shadingDescId = 3;
        
    /// multi-circles
    //shaderingEntitiesPool[4].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::MultiCircle);
    shaderingEntitiesPool[4].drawUnitId    = drawingStorage->getIdWithType(DrawingUnitType::strokeShape);
    shaderingEntitiesPool[4].shadingDescId = 4;
    auto drawUnitId_4                      = drawingStorage->getIdWithName("ship01.glsl");
    shaderingEntitiesPool[5].drawUnitId    = drawUnitId_4;
    shaderingEntitiesPool[5].shadingDescId = 5;

    /// circle
    //entities[0].shadingId = 4;

    /// circle
    entities[0].shadingId = 0;
    ///// circle
    entities[1].shadingId = 1;
    /// circle
    entities[2].shadingId = 2;
    /// ring
    entities[3].shadingId = 3;
    /// multi-circles
    entities[4].shadingId = 4;
    /// ship
    entities[5].shadingId = 5;


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

        bvh->addItem(et.id, bounds);
    }
    bvh->build();
}

int EntityRenderSystem::drawQuery(const Math::VxRect& wbounds, int phase)
{
    queriedEIds.clear();
    bvh->queryBounds(wbounds, queriedEIds);

    //if (phase < 2)
    //{
    //    printf("EntityRenderSystem::drawQuery() A size: %d, phase: %d, bounds total: %d\n", queriedEIds.size(), phase, bvh.getBoundsCapacity());
    //    if (phase < 2)
    //    {
    //        printf("wbounds, ");
    //        wbounds.print();
    //        auto b = bvh.getBoundsAt(0);
    //        printf("b, ");
    //        b.print();
    //    }
    //}

    /// for test
    auto flag = false;
    if (queriedEIds.empty())
    {
        flag = true;
    }
    return static_cast<int>(queriedEIds.size());
}
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    if (!compStorage)
        return;

    //printf("EntityRenderSystem::render() B %d\n", queriedEIds.size());
    /// 这里是正确的写法
    if (queriedEIds.empty())
        return;

    auto  total    = queriedEIds.size();
    auto&  entities  = compStorage->entities;
    size_t  drawTotal    = 0;
    for (auto i = 0; i < total; i++)
    {
        auto& et = entities[queriedEIds[i]];
        if (et.shadingId < 0 || !et.visible)
            continue;
        auto flag = drawUnit(et, vpM, wbounds);
        drawTotal += flag ? 1 : 0;
    }
    if (drawTotal < total)
    {
        printf(">>> >>> >>> EntityRenderSystem::render() , drawTotal: %d, total: %d\n", drawTotal, total);
    }
    return;

    // 暂时这样写，以便测试dragging
    auto& ets = compStorage->entities;
    auto  tot = ets.size();
    for (auto i = 0; i < tot; i++)
    {
        auto& et = ets[i];
        if (et.shadingId < 0 || !et.visible)
            continue;
        drawUnit(et, vpM, wbounds);
    }
}

bool EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    if (!compStorage)
        return false;

    auto& shaderingDescPool = compStorage->shaderingDescPool;

    const auto&  shadingEt = compStorage->get<Component::UnitShadingEntity>(entity.shadingId);
    //auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto& drs  = *drawingStorage;
    auto&        drawUnit  = drs[shadingEt.drawUnitId];
    auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
    auto& trans     = shdDesc.transform;

    Math::Bounds vb;
    vb.setXYWH(trans.x, trans.y, trans.sx, trans.sy);
    if (!wbounds.intersects(vb))
        return false;

    //printf("xxx xxx trans(x=%f, y=%f)\n", trans.x, trans.y);

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdDesc.color);
    drawUnit.objMat.setXY(trans.x, trans.y);
    drawUnit.objMat.setScaleXY(trans.sx, trans.sy);
    drawUnit.mvp = vpM;
    drawUnit.draw();
    return true;
}
void EntityRenderSystem::clear()
{
}
} // namespace Voxol::Render