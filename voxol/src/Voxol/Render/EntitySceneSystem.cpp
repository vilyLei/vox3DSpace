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

    //Math::Bounds                           bounds{0,0,1,1};
    //Math::Bounds                           vb{};

    entitiesPool.forEach([&](auto& et, uint32_t index) {

        if (et.shadingId == Component::INVALID_ID)
            return;

        //auto& trans = transformsPool[et.transformId];
        //bounds.setXYWH(trans.x, trans.y, trans.sx, trans.sy);
        //bounds.mat33MapTo(storage->entityGlobalMat33Map[et.id], vb);
        auto&& vb = storage->getEntityGlobalBoundsAt( et.id );
        bvh->addItem(et.id, vb);
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

const std::vector<uint32_t> EntitySceneSystem::getQueriedEIds() const
{
    return queriedEIds;
}
void EntitySceneSystem::clear()
{
}

void EntitySceneSystem::updateBVHBoundsWithEntityId(uint32_t eId)
{
    if (eId == Component::INVALID_ID)
        return;

    std::vector<uint32_t> ids{};
    entityStorage->comp->getIdsFromId(eId, ids);
    for (auto pid : ids)
    {
        bvh->updateItemBoundsByObjectId(pid, entityStorage->comp->getEntityGlobalBoundsAt(pid));
    }
}
} // namespace Voxol::Render