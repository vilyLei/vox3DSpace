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

    // for test
    entityStorage01 = EntityUnitStorage::make();
    entityStorage01->initalizeFromFile("");


    entityStorage = EntityUnitStorage::make();
    entityStorage->initalize(512);
    
    auto& entitiesPool          = entityStorage->comp->entitiesPool;
    auto& shaderingEntitiesPool = entityStorage->comp->shaderingEntitiesPool;
    auto& shaderingDescPool     = entityStorage->comp->shaderingDescPool;
    auto& transformPool         = entityStorage->comp->transformPool;

    Math::Bounds                           bounds{};

    entitiesPool.forEach([&](auto& et, int32_t index) {

        if (et.shadingId < 0)
            return;

        auto& trans     = transformPool[et.transformId];
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
void EntitySceneSystem::clear()
{
}
} // namespace Voxol::Render