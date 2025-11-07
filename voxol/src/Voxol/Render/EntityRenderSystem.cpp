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

    auto& compStorage = entityStorage->comp;

    auto   total        = queriedEIds.size();
    auto&  entitiesPool = compStorage->entitiesPool;
    size_t drawTotal    = 0;

    //printf("EntityRenderSystem::render() total: %zu\n", total);

    for (auto i = 0; i < total; i++)
    {
        auto&& key = queriedEIds[i];

        //printf("i: %zu, render key: %s\n",i, key.toString().c_str());

        if (key.isIDInvalid())
            continue;
        if (key.flags() > 0)
        {
            drawUnitEffect(key, vpM, wbounds);
            continue;
        }
        auto proId = key.protoId();
        if (ID::isInvalidID(proId))
        {
            continue;
        }
        if (entitiesPool.isInvalid(proId))
        {
            continue;
        }
        auto  iid = key.iid();
        auto& et  = entitiesPool[proId];
        if (Render::ID::isInvalidID(et.shadingId) || !et.visible)
        {
            continue;
        }

        //printf("EntityRenderSystem::render() ...\n");
        if (key.isIIDValid())
        {
            auto&& wmat = compStorage->entityInsGlobalMat33Map[key];
            auto   flag = drawUnit(et, vpM, wbounds, wmat);
            drawTotal += flag ? 1 : 0;
            //printf("drawUnit with prototype child rendering process ...\n");
            continue;
        }

        auto flag = drawUnit(et, vpM, wbounds, compStorage->entityGlobalMat33Map[proId]);
        drawTotal += flag ? 1 : 0;
    }
    //if (drawTotal < total)
    //{
    //    printf(">>> >>> >>> EntityRenderSystem::render() , drawTotal: %d, total: %d\n", drawTotal, total);
    //}
    //return;
    //auto& ets = compStorage->entitiesPool;
    //ets.forEach([&](auto& et) {
    //    if (ID::isValidID(et.prototypeId))
    //    {
    //        printf("drawUnit with prototype process ...\n");
    //        return;
    //    }
    //    if (et.shadingId < 0 || !et.visible)
    //        return;
    //    drawUnit(et, vpM, wbounds);
    //});
}
bool EntityRenderSystem::drawUnitEffect(const ID::KeyUint64 etKey, const Math::Mat33& vpM, const Math::Bounds& wbounds)
{
    auto&  compStorage = entityStorage->comp;
    auto&& srUnit      = compStorage->effectShadowEntityMap[etKey];

    if (!compStorage->effectShadowMap.contains(srUnit.effectId))
        return false;

    auto&& entityId          = srUnit.entityId;
    auto&& entity            = compStorage->entitiesPool[entityId.protoId()];
    auto   etId              = entity.id;
    auto&  shaderingEntities = compStorage->shaderingEntitiesPool;
    auto&  shaderingDescVec  = compStorage->shaderingDescPool;

    auto& shadingEt = shaderingEntities[entity.shadingId];
    auto& shdDesc   = shaderingDescVec[shadingEt.shadingDescId];

    if (shdDesc.flags == 0)
        return false;

    auto&& shdData = compStorage->effectShadowMap[srUnit.effectId];

    auto wm = compStorage->getEntityGlobalMat33At(entityId);

    // shadow offset in the global space
    wm.offsetXY(shdData.offset);

    Math::Bounds vb;
    Component::defaultRect.mat33MapTo(wm, vb);
    if (!wbounds.intersects(vb))
        return false;

    auto&  modelsPool = compStorage->modelsPool;
    auto   drawingId  = modelsPool[entity.modelId].drawUnitId;
    auto&  drs        = *entityStorage->drawing;
    auto&& drawUnit   = drs[drawingId];

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdData.color);
    drawUnit.objMat = wm;
    drawUnit.mvp    = vpM;
    drawUnit.draw();
}

bool EntityRenderSystem::drawUnit(const Component::UnitEntity& entity, const Math::Mat33& vpM, const Math::Bounds& wbounds, const Math::Mat33& wM)
{
    auto& compStorage       = entityStorage->comp;
    auto& shaderingDescPool = compStorage->shaderingDescPool;
    auto& transformsPool    = compStorage->transformsPool;
    auto& modelsPool        = compStorage->modelsPool;

    auto&& shadingEt = compStorage->get<Component::UnitShadingEntity>(entity.shadingId);
    auto   drawingId = modelsPool[entity.modelId].drawUnitId;
    auto&  drs       = *entityStorage->drawing;
    auto&& drawUnit  = drs[drawingId];
    auto&& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];

    Math::Bounds vb;
    Component::defaultRect.mat33MapTo(wM, vb);
    if (!wbounds.intersects(vb))
        return false;

    //if (shdDesc.flags > 0 && compStorage->shadingShadowIdMap.contains(shadingEt.shadingDescId))
    //{
    //    auto&& effects = compStorage->shadingShadowIdMap[shadingEt.shadingDescId];
    //    auto   tot     = effects.size();
    //    // draw shadows
    //    for (auto i = 0; i < tot; i++)
    //    {
    //        auto&& shd         = compStorage->effectShadowMap[effects[i]];
    //        auto mat           = wM;
    //        mat.offsetXY(shd.offset);
    //        drawUnit.blendMode = 1;
    //        drawUnit.setColor(shd.color);
    //        drawUnit.objMat = mat;
    //        drawUnit.mvp    = vpM;
    //        drawUnit.draw();
    //    }
    //}

    //printf("xxx xxx trans(x=%f, y=%f)\n", trans.x, trans.y);

    drawUnit.blendMode = 1;
    drawUnit.setColor(shdDesc.color);
    drawUnit.objMat = wM;
    drawUnit.mvp    = vpM;
    drawUnit.draw();
    return true;
}

void EntityRenderSystem::clear()
{
}

} // namespace Voxol::Render