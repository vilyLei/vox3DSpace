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
}
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds, const std::vector<uint32_t>& queriedEIds)
{
    if (!entityStorage)
        return;

    //printf("EntityRenderSystem::render() B %d\n", queriedEIds.size());
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
        printf(">>> >>> >>> EntityRenderSystem::render() , drawTotal: %d, total: %d\n", drawTotal, total);
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

bool EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    if (!entityStorage)
        return false;


    auto  compStorage       = entityStorage->comp;
    auto& shaderingDescPool = compStorage->shaderingDescPool;
    auto& transformsPool = compStorage->transformsPool;
    auto& modelsPool        = compStorage->modelsPool;

    const auto& shadingEt = compStorage->get<Component::UnitShadingEntity>(entity.shadingId);
    //auto& drawUnit  = drawingUnits[shadingEt.drawUnitId];
    auto  drawingId = modelsPool[entity.modelId].drawUnitId;
    auto& drs      = *entityStorage->drawing;
    auto& drawUnit  = drs[drawingId];
    auto& shdDesc  = shaderingDescPool[shadingEt.shadingDescId];
    //auto& trans    = transformsPool[entity.transformId];
    auto&&       wmat = compStorage->entityGlobalMat33Map[entity.id];
    Math::Bounds vbUnit{0,0,1,1};
    Math::Bounds vb;
    vbUnit.mat33MapTo(wmat, vb);
    //vb.setXYWH(trans.x, trans.y, trans.sx, trans.sy);
    if (!wbounds.intersects(vb))
        return false;

    //printf("xxx xxx trans(x=%f, y=%f)\n", trans.x, trans.y);

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdDesc.color);
    //drawUnit.objMat.setXY(trans.x, trans.y);
    //drawUnit.objMat.setScaleXY(trans.sx, trans.sy);
    drawUnit.objMat = wmat;
    drawUnit.mvp = vpM;
    drawUnit.draw();
    return true;
}
void EntityRenderSystem::clear()
{
}

} // namespace Voxol::Render