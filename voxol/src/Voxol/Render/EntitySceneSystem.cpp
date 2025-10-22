#include "EntitySceneSystem.h"

namespace Voxol::Render
{

EntitySceneSystem::SP EntitySceneSystem::make()
{
    auto sp = std::make_shared<EntitySceneSystem>();
    return sp;
}

void EntitySceneSystem::initalize()
{
    if (entityStorage)
        return;
    entityStorage = EntityUnitStorage::make();
    entityStorage->initalize(512);
    
    auto& entitiesPool          = entityStorage->comp->entitiesPool;
    auto& shaderingEntitiesPool = entityStorage->comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = entityStorage->comp->shaderingDescPool;

    Math::Bounds                           bounds{};

    entitiesPool.forEach([&](auto& et, int32_t index) {

        if (et.shadingId < 0)
            return;

        auto& shadingEt = shaderingEntitiesPool[et.shadingId];
        auto& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
        auto& trans     = shdDesc.transform;
        bounds.setXYWH(trans.x, trans.y, trans.sx, trans.sy);

        bvh->addItem(et.id, bounds);
    });

    bvh->build();
}

int EntitySceneSystem::drawQuery(const Math::VxRect& wbounds, int phase)
{
    queriedEIds.clear();
    bvh->queryBounds(wbounds, queriedEIds);

    //if (phase < 2)
    //{
    //    printf("EntitySceneSystem::drawQuery() A size: %d, phase: %d, bounds total: %d\n", queriedEIds.size(), phase, bvh.getBoundsCapacity());
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

const std::vector<int32_t> EntitySceneSystem::getQueriedEIds() const
{
    return queriedEIds;
}
/*
void EntitySceneSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    if (!entityStorage)
        return;

    //printf("EntitySceneSystem::render() B %d\n", queriedEIds.size());
    /// 这里是正确的写法
    if (queriedEIds.empty())
        return;

    auto compStorage = entityStorage->comp;

    auto   total     = queriedEIds.size();
    auto&  entitiesPool = compStorage->entitiesPool;
    size_t drawTotal = 0;

    /// 注意, 这里要基于渲染顺序排序之后再绘制

    for (auto i = 0; i < total; i++)
    {
        auto& et = entitiesPool[queriedEIds[i]];
        if (et.shadingId < 0 || !et.visible)
            continue;
        auto flag = drawUnit(et, vpM, wbounds);
        drawTotal += flag ? 1 : 0;
    }
    if (drawTotal < total)
    {
        printf(">>> >>> >>> EntitySceneSystem::render() , drawTotal: %d, total: %d\n", drawTotal, total);
    }
    return;

    // 暂时这样写，以便测试dragging
    auto& ets = compStorage->entitiesPool;
    ets.forEach([&](auto& et) {
        if (et.shadingId < 0 || !et.visible)
            return;
        drawUnit(et, vpM, wbounds);
    });
}

bool EntitySceneSystem::drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    if (!entityStorage)
        return false;


    auto  compStorage       = entityStorage->comp;
    auto& shaderingDescPool = compStorage->shaderingDescPool;

    const auto& shadingEt = compStorage->get<Component::UnitShadingEntity>(entity.shadingId);
    //auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto& drs      = *entityStorage->drawing;
    auto& drawUnit = drs[shadingEt.drawUnitId];
    auto& shdDesc  = shaderingDescPool[shadingEt.shadingDescId];
    auto& trans    = shdDesc.transform;

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
//*/
void EntitySceneSystem::clear()
{
}
} // namespace Voxol::Render