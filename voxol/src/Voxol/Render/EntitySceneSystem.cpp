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
    
    auto& storage                = entityStorage->comp;
    auto& entitiesPool           = storage->entitiesPool;
    auto& shaderingEntitiesPool  = storage->shaderingEntitiesPool;
    auto& shaderingDescPool      = storage->shaderingDescPool;
    auto& transformsPool         = storage->transformsPool;

    auto& entityGlobalMat33Map = storage->entityGlobalMat33Map;
    auto updateProtoEtBVHData = [&](auto& et) {
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        bvh->addItem(ID::KeyUint64::make(et.id, 0), vb);

        auto& worldMat = entityGlobalMat33Map[et.id];

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

        auto&& vb = storage->getEntityGlobalBoundsAt( et.id );
        bvh->addItem(ID::KeyUint64::make(et.id, 0), vb);
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

void EntitySceneSystem::updateBVHBoundsWithEntityId(uint32_t eId)
{
    if (ID::isInvalidID(eId))
        return;

    std::vector<ID::KeyUint64> ids{};
    entityStorage->comp->getIdsFromId(eId, ids);
    for (auto pid : ids)
    {
        bvh->updateItemBoundsByObjectId(pid, entityStorage->comp->getEntityGlobalBoundsAt(pid.id()));
    }
    bvh->updateDirty();
}
} // namespace Voxol::Render