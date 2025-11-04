#include "EntitySystemLayer.h"

namespace Voxol::System
{
EntitySystemLayer::SP EntitySystemLayer::make()
{
    auto sp = std::make_shared<EntitySystemLayer>();
    return sp;
}

void EntitySystemLayer::updateTileWithEntityId(const Render::ID::KeyUint64& eId)
{
    if (eId.isIDInvalid())
        return;

    auto&                 etCompStorage = etSceneSys->entityStorage->comp;
    auto&                 bvh           = etSceneSys->bvh;
    std::vector<Render::ID::KeyUint64> ids{};
    //etCompStorage->getIdsFromId(eId.protoId(), ids);
    Math::Bounds vb;
    etCompStorage->collectAllEntities(eId, ids);
    for (auto pid : ids)
    {
        tileSys->addDirtyBounds(bvh->getBoundsAt(pid), 0);
    }
}
void EntitySystemLayer::updateBVHAndTileWithEntityId(const Render::ID::KeyUint64& eId)
{
    if (eId.isIDInvalid())
        return;

    auto&  etCompStorage = etSceneSys->entityStorage->comp;
    auto& bvh           = etSceneSys->bvh;

    auto&& parentMat = etCompStorage->getEntityParentGlobalMatAt(eId.protoId());
    etCompStorage->traverseBuildGlobalMat(eId.protoId(), parentMat);
    etCompStorage->updateAllInstanceGlobalMats(eId);
    Math::Bounds                       vb;
    std::vector<Render::ID::KeyUint64> ids{};
    etCompStorage->collectAllEntities(eId, ids);
    auto& wInsMats = etCompStorage->entityInsGlobalMat33Map;
    for (auto pid : ids)
    {
        if (pid.iid() > 0)
        {
            Render::Component::defaultRect.mat33MapTo(wInsMats[pid], vb);
        }
        else
        {
            vb = etCompStorage->getEntityGlobalBoundsAt(pid.protoId());
        }

        tileSys->addDirtyBounds(vb, 1);
        bvh->updateItemBoundsByObjectId(pid, vb);
    }
    bvh->updateDirty();
}
void EntitySystemLayer::initalize(const std::string& configFileName)
{
    etSceneSys->initalize(configFileName);
    etRenderSys->entityStorage = etSceneSys->entityStorage;
    etRenderSys->initalize();
    tileSys->initalize();

    auto& etCompStorage = etSceneSys->entityStorage->comp;
    
    uiOpLayer                      = std::make_shared<System::UIOperationLayer>();
    uiOpLayer->mouseCtrl.dirtyCall = [&, this](const Math::Bounds& bounds, uint32_t type, const Render::ID::KeyUint64& etId) {
        if (type == 0)
        {
            updateTileWithEntityId(etId);
        }
        else
        {
            updateBVHAndTileWithEntityId(etId);
        }
    };
    uiOpLayer->etSceneSys = etSceneSys;
    uiOpLayer->initialize();

    auto queryCall = [this](const Math::VxRect& bounds, int phase) -> int {
        return etSceneSys->drawQuery(bounds, phase);
    };
    auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) {
        auto&& ids = etSceneSys->getQueriedEIds();
        etRenderSys->render(drawCtx, vpMat, bounds, ids);
    };

    drawCtx.drawCall      = drawCall;
    drawCtx.drawQueryCall = queryCall;

    uiOpLayer->shortcutMana.registerShortcut(
        {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_Z}, [this] {
            //std::cout << "[Undo] Ctrl + Z pressed\n";
            undo();
        },
        System::ShortcutManager::TriggerType::Press);
    uiOpLayer->shortcutMana.registerShortcut({GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_SHIFT, GLFW_KEY_Y}, [] {
        std::cout << "Ctrl + Shift + Y pressed\n";
    });
}


void EntitySystemLayer::updateCtx(const Render::Draw::DrawContext& ctx)
{
    drawCtx.clearParam = ctx.clearParam;
    drawCtx.drawParam  = ctx.drawParam;
}

void EntitySystemLayer::undo()
{
    auto& storage     = etRenderSys->entityStorage;
    auto& compStorage = storage->comp;
    auto&& itemData    = compStorage->historyManager->popItem();
    printf("EntitySystemLayer::undo() itemData.id: %u\n", itemData.id.id());
    if (itemData.id.id() < 0)
    {
        return;
    }

    printf("EntitySystemLayer::undo() update some items.\n");
    auto&& etrans = compStorage->getEntityTransformAt(itemData.id.id());

    Math::Vec2 pv{itemData.trans.x, itemData.trans.y};

    compStorage->setEntityLocalXYAt(pv, itemData.id.id());

    auto& bvh = etSceneSys->bvh;
    auto b0  = bvh->getBoundsAt(itemData.id);
    auto b1  = b0;
    if (tileSys)
    {
        tileSys->addDirtyBounds(b0, 0);
        b1.moveTo(pv.x, pv.y);
        tileSys->addDirtyBounds(b1, 1);
    }
    if (bvh)
    {
        bvh->updateItemBoundsByObjectId(itemData.id, b1);
        bvh->updateDirty();
    }
}

void EntitySystemLayer::render(const Math::Mat33& vpMat)
{
    tileSys->run(drawCtx);
}

void EntitySystemLayer::updateKeyboardParams(int key, int scancode, int action, int mods)
{
    uiOpLayer->updateKeyboardParams(key, scancode, action, mods);
}

void EntitySystemLayer::updateMouseParams(const System::Mouse::MouseInputParam& param)
{
    uiOpLayer->updateMouseParams(drawCtx, param);
}
} // namespace Voxol::System