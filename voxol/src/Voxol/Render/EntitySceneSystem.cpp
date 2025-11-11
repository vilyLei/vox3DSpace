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
    auto& instanceStorage       = storage->instanceStorage;


    auto addShadowEffectBVHData = [&](const ID::KeyUint64& key, const Math::Mat33& wmat) {
        auto protoId = key.protoId();
        if (entitiesPool.isInvalid(protoId)) { return; }

        auto&& et        = entitiesPool[protoId];
        auto&  shadingEt = shaderingEntitiesPool[et.shadingId];
        auto&  desc      = shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = storage->shadingShadowIdMap[shadingEt.shadingDescId];

        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = storage->effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            Component::defaultRect.mat33MapTo(wm, vb);
            auto shadowKey = ID::KeyUint64::makeWithEffectShadow(key, ef);
            bvh->addItem(shadowKey, vb);
            storage->effectShadowEntityMap[shadowKey] = {shadowKey, key, ef};
        }
    };

    auto updateProtoEtBVHData = [&](auto& et) {
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        bvh->addItem(ID::KeyUint64::make(et.id), vb);
        auto& wmats = storage->entityInsGlobalMat33Map;
        if (!instanceStorage.contains(et.id))
            return;

        auto&& insMap = instanceStorage[et.id];
        for (auto& item : insMap.map)
        {
            auto&& mat = wmats[item.first];
            addShadowEffectBVHData(item.first, mat);
            Component::defaultRect.mat33MapTo(mat, vb);
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

        auto&& key = ID::KeyUint64::make(et.id);
        addShadowEffectBVHData(key, storage->getEntityGlobalMat33At(et.id));
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

    //for (auto key : queriedEIds)
    //{
    //    printf("drawQuery(). key: %s\n", key.toString().c_str());
    //}
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

    auto& compst = entityStorage->comp;

    auto addShadowEffectBVHData = [&](const ID::KeyUint64& key) {
        auto protoId = key.protoId();
        if (compst->entitiesPool.isInvalid(protoId)) { return; }

        auto&& et        = compst->entitiesPool[protoId];
        auto&& shadingEt = compst->shaderingEntitiesPool[et.shadingId];
        auto&& desc      = compst->shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = compst->shadingShadowIdMap[shadingEt.shadingDescId];

        auto&& wmat = compst->getEntityGlobalMat33At(key);

        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = compst->effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            Component::defaultRect.mat33MapTo(wm, vb);

            auto&& efKey = ID::KeyUint64::makeWithEffectShadow(key, ef);
            auto   pos   = wm.getXY();
            //printf("efKey: %s\n", efKey.idToString().c_str());
            //vb.print();
            //printf("        pos(x=%f,y=%f), offset(x=%f,y=%f)\n", pos.x, pos.y, shdData.offset.x, shdData.offset.y);

            bvh->updateItemBoundsByObjectId(efKey, vb);
            callback(key, vb);
        }
    };

    Math::Bounds vb;

    std::vector<ID::KeyUint64> ids{};
    compst->collectAllEntities(ID::KeyUint64::make(eId), ids);
    for (auto pid : ids)
    {
        if (pid.flags() > 0)
            continue;

        auto wm = compst->getEntityGlobalMat33At(pid);
        addShadowEffectBVHData(pid);
        Component::defaultRect.mat33MapTo(wm, vb);
        bvh->updateItemBoundsByObjectId(pid, vb);
        callback(pid, vb);
    }
    //bvh->updateDirty();
}
} // namespace Voxol::Render