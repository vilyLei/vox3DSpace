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
void EntityRenderSystem::render(const Draw::DrawContext& rctx, const Math::Mat33& vpM, const Math::Bounds& wbounds, const std::vector<ID::KeyUint64>& queriedEIds)
{
    if (!entityStorage)
        return;

    //printf("EntityRenderSystem::render() B %d\n", queriedEIds.size());

    if (queriedEIds.empty())
        return;

    auto compStorage = entityStorage->comp;

    auto   total     = queriedEIds.size();
    auto&  entitiesPool = compStorage->entitiesPool;
    size_t drawTotal = 0;

    for (auto i = 0; i < total; i++)
    {
        if (queriedEIds[i].isIDInvalid())
            continue;
        auto iid = queriedEIds[i].iid();
        if (iid > 0)
        {
            printf("drawUnit with prototype child rendering process ...\n");
            continue;
        }

        auto& et = entitiesPool[queriedEIds[i].id()];
        if (ID::isValidID(et.prototypeId))
        {
            printf("drawUnit with prototype root rendering process ...\n");
            //return;
        }
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

    auto& ets = compStorage->entitiesPool;
    ets.forEach([&](auto& et) {
        if (ID::isValidID(et.prototypeId))
        {
            printf("drawUnit with prototype process ...\n");
            return;
        }
        if (et.shadingId < 0 || !et.visible)
            return;
        drawUnit(et, vpM, wbounds);
    });
}

bool EntityRenderSystem::drawUnitWithPrototype(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{

    return true;
}
bool EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{

    auto&  compStorage       = entityStorage->comp;
    auto& shaderingDescPool = compStorage->shaderingDescPool;
    auto& transformsPool = compStorage->transformsPool;
    auto& modelsPool        = compStorage->modelsPool;

    const auto& shadingEt = compStorage->get<Component::UnitShadingEntity>(entity.shadingId);
    auto  drawingId = modelsPool[entity.modelId].drawUnitId;
    auto& drs      = *entityStorage->drawing;
    auto& drawUnit  = drs[drawingId];
    auto& shdDesc  = shaderingDescPool[shadingEt.shadingDescId];
    auto&&       wmat = compStorage->entityGlobalMat33Map[entity.id];

    Math::Bounds vb;
    Component::defaultRect.mat33MapTo(wmat, vb);
    if (!wbounds.intersects(vb))
        return false;

    //printf("xxx xxx trans(x=%f, y=%f)\n", trans.x, trans.y);

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdDesc.color);
    drawUnit.objMat = wmat;
    drawUnit.mvp = vpM;
    drawUnit.draw();
    return true;
}
void EntityRenderSystem::clear()
{
}

} // namespace Voxol::Render