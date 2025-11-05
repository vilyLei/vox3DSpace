#include "EntitySceneSystem.h"
#include <algorithm>

namespace Voxol::Render
{

EntitySceneSystem::SP EntitySceneSystem::make()
{
    auto sp = std::make_shared<EntitySceneSystem>();
    return sp;
}

void EntitySceneSystem::initalize(const std::string& configFileName)
{
    if (entityStorage)
        return;

    // for test
    //entityStorage01 = EntityUnitStorage::make();
    //entityStorage01->initalizeFromFile("");

    entityStorage = EntityUnitStorage::make();
    if (configFileName.empty())
    {
        entityStorage->initalize(512);
    }
    else
    {
        entityStorage->initalizeFromFile(configFileName);
    }

    auto& storage               = entityStorage->comp;
    auto& entitiesPool          = storage->entitiesPool;
    auto& shaderingEntitiesPool = storage->shaderingEntitiesPool;
    auto& shaderingDescPool     = storage->shaderingDescPool;
    auto& transformsPool        = storage->transformsPool;
    auto& insStorage            = storage->insStorage;


    auto addShadowEffectBVHData = [&](const ID::KeyUint64& key, const Math::Mat33& wmat) {
        auto protoId = key.protoId();
        if (entitiesPool.isInvalid(protoId)) { return; }

        auto&& et        = entitiesPool[protoId];
        auto&  shadingEt = shaderingEntitiesPool[et.shadingId];
        auto&  desc      = shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = storage->effectShadowIdMap[shadingEt.shadingDescId];

        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = storage->effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            Component::defaultRect.mat33MapTo(wm, vb);
            bvh->addItem(ID::KeyUint64::makeWithEffectShadow(key, ef), vb);
        }
    };

    auto updateProtoEtBVHData = [&](auto& et) {
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        bvh->addItem(ID::KeyUint64::make(et.id), vb);
        auto& wmats = storage->entityInsGlobalMat33Map;

        auto&& insMap = insStorage[et.id];
        for (auto& item : insMap.map)
        {
            //auto& ins = item.second;
            auto&& mat = wmats[item.first];
            addShadowEffectBVHData(item.first, mat);
            //Component::defaultRect.mat33MapTo(ins.worldMat, vb);
            Component::defaultRect.mat33MapTo(mat, vb);
            //bvh->addItem(ins.id, vb);
            bvh->addItem(item.first, vb);
        }
    };

    entitiesPool.forEach([&](auto& et, uint32_t index) {

        if (ID::isInvalidID(et.transformId))
            return;

        if (ID::isValidID(et.prototypeId))
        {
            updateProtoEtBVHData(et);
            return;
        }

        //auto& trans = transformsPool[et.transformId];
        //bounds.setXYWH(trans.x, trans.y, trans.sx, trans.sy);
        //bounds.mat33MapTo(storage->entityGlobalMat33Map[et.id], vb);

        auto&& key = ID::KeyUint64::make(et.id);
        addShadowEffectBVHData(key, storage->getEntityGlobalMatAt(et.id));
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        bvh->addItem(key, vb);
    });

    bvh->build();
}

int EntitySceneSystem::drawQuery(const Math::VxRect& wbounds, int phase)
{
    queriedEIds.clear();
    bvh->queryBounds(wbounds, queriedEIds);

    entityStorage->comp->checkIds(queriedEIds);
    return static_cast<int>(queriedEIds.size());
}

const std::vector<ID::KeyUint64> EntitySceneSystem::getQueriedEIds() const
{
    return queriedEIds;
}
void EntitySceneSystem::clear()
{
}

void EntitySceneSystem::updateBVHBoundsWithEntityId(uint32_t eId, BoundsUpdateCallType callback)
{
    if (ID::isInvalidID(eId))
        return;

    auto& compst            = entityStorage->comp;

    auto addShadowEffectBVHData = [&](const ID::KeyUint64& key, const Math::Mat33& wmat) {
        auto protoId = key.protoId();
        if (compst->entitiesPool.isInvalid(protoId)) { return; }

        auto&& et        = compst->entitiesPool[protoId];
        auto&  shadingEt = compst->shaderingEntitiesPool[et.shadingId];
        auto&  desc      = compst->shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = compst->effectShadowIdMap[shadingEt.shadingDescId];

        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = compst->effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            Component::defaultRect.mat33MapTo(wm, vb);
            bvh->addItem(ID::KeyUint64::makeWithEffectShadow(key, ef), vb);
            callback(key, vb);
        }
    };

    Math::Bounds vb;

    std::vector<ID::KeyUint64> ids{};
    //compst->getIdsFromId(eId, ids);
    compst->collectAllEntities(ID::KeyUint64::make(eId), ids);
    for (auto pid : ids)
    {
        if (pid.flags() > 0)
            continue;

        if (pid.isIIDValid())
        {
            auto&& wm = compst->entityInsGlobalMat33Map[pid];
            addShadowEffectBVHData(pid, wm);

            Component::defaultRect.mat33MapTo(wm, vb);
            bvh->updateItemBoundsByObjectId(pid, vb);
            callback(pid, vb);
        }
        else
        {
            vb = compst->getEntityGlobalBoundsAt(pid.protoId());
            addShadowEffectBVHData(pid, compst->getEntityGlobalMat33At(pid.protoId()));
            bvh->updateItemBoundsByObjectId(pid, vb);
            callback(pid, vb);
        }
    }
    bvh->updateDirty();
}
} // namespace Voxol::Render