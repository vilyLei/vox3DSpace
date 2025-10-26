#include "EntitySystemLayer.h"

namespace Voxol::System
{
EntitySystemLayer::SP EntitySystemLayer::make()
{
    auto sp = std::make_shared<EntitySystemLayer>();
    return sp;
}
void EntitySystemLayer::initalize()
{
}
void EntitySystemLayer::undo()
{
    ///*
    auto storage     = etRenderSys->entityStorage;
    auto compStorage = storage->comp;
    auto itemData    = compStorage->historyManager->popItem();
    printf("EntitySystemLayer::undo()£¬ itemData.id: %d\n", itemData.id);
    if (itemData.id < 0)
    {
        return;
    }

    printf("EntitySystemLayer::undo()£¬ update some items.\n");
    auto etrans = compStorage->getEntityTransformAt(itemData.id);

    Math::Vec2 pv{itemData.trans.x, itemData.trans.y};

    compStorage->setEntityXYAt(pv, itemData.id);

    auto bvh = etSceneSys->bvh;
    auto b0  = bvh->getBoundsAt(itemData.id);
    auto b1  = b0;
    if (tileSys)
    {
        // ÒÆ³ö
        tileSys->addDirtyBounds(b0, 0);
        b1.moveTo(pv.x, pv.y);
        // ÒÆÈë
        tileSys->addDirtyBounds(b1, 1);
    }
    if (bvh)
    {
        bvh->updateItemBoundsByObjectId(itemData.id, b1);
        bvh->updateDirty();
    }
}
void EntitySystemLayer::render(const Math::Mat33& vpMat) {

}
} // namespace Voxol::System