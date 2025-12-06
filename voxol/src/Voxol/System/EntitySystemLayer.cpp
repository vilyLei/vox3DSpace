#include "EntitySystemLayer.h"

namespace Voxol::System
{
EntitySystemLayer::SP EntitySystemLayer::make()
{
    auto sp = std::make_shared<EntitySystemLayer>();
    return sp;
}

void EntitySystemLayer::updateTileWithBVHBoundsAndEntityId(const Base::ID::KeyUint64& eId, int type)
{

    if (eId.isIDInvalid())
        return;

    auto& compStorage         = etSceneSys->entityStorage->comp;

    auto upateEffectBoundsData = [&, this](const Base::ID::KeyUint64& key, int type) {
        auto protoId = key.protoId();
        if (compStorage->entitiesPool.isInvalid(protoId)) { return; }

        auto&& et = compStorage->entitiesPool[protoId];
        if (Base::ID::isInvalidID(et.shadingId)) { return; }

        auto&& shadingEt = compStorage->shaderingEntitiesPool[et.shadingId];
        if (compStorage->entitiesPool.isInvalid(shadingEt.shadingDescId)) { return; }

        auto&& desc = compStorage->shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = compStorage->shadingShadowIdMap[shadingEt.shadingDescId];

        //auto&& wmat = compStorage->getEntityGlobalMat33At(key);

        //Math::Bounds vb;
        for (auto& ef : efs)
        {
            //auto&& shdData = compStorage->effectShadowMap[ef];
            //auto   wm      = wmat;
            //wm.offsetXY(shdData.offset);
            //srcBounds.mat33MapTo(wm, vb);

            auto&& efKey = Base::ID::KeyUint64::makeWithEffectShadow(key, ef);
            auto&& bounds = etSceneSys->bvh->getBoundsAt(efKey);
            //callback(key, vb);
            tileSys->addDirtyBounds(bounds, type);
        }
    };

    //Math::Bounds vb;

    std::vector<Base::ID::KeyUint64> ids{};
    compStorage->collectAllEntities(eId, ids);
    for (auto pid : ids)
    {
        if (pid.flags() > 0)
            continue;

        //auto&& srcBounds = compStorage->getEntityLocalBoundsAt(pid);
        //auto   wm        = compStorage->getEntityGlobalMat33At(pid);
        upateEffectBoundsData(pid, type);
        //srcBounds.mat33MapTo(wm, vb);
        //callback(pid, vb);

        auto&& bounds = etSceneSys->bvh->getBoundsAt(pid);
        //callback(key, vb);
        tileSys->addDirtyBounds(bounds, type);
    }
}
void EntitySystemLayer::updateTileWithEntityId(const Base::ID::KeyUint64& eId)
{
    if (eId.isIDInvalid())
        return;

    //etSceneSys->updateBoundsWithEntityId(eId.protoId(), [this](const Base::ID::KeyUint64& etId, const Math::Bounds& bounds) {
    //    tileSys->addDirtyBounds(bounds, 0);
    //});
    updateTileWithBVHBoundsAndEntityId(eId, 0);
}
void EntitySystemLayer::updateBVHAndTileWithEntityId(const Base::ID::KeyUint64& eId)
{
    if (eId.isIDInvalid())
        return;

    auto&  etCompStorage = etSceneSys->entityStorage->comp;
    //auto&& parentMat     = etCompStorage->getEntityParentGlobalMatAt(eId.protoId());
    //etCompStorage->traverseBuildGlobalMat(eId.protoId(), parentMat);
    etCompStorage->traverseBuildGlobalMat(eId.protoId());

    etCompStorage->updateAllInstanceGlobalMats(eId);

    etSceneSys->updateBoundsWithEntityId(eId.protoId(), [this](const Base::ID::KeyUint64& etId, const Math::Bounds& bounds) {
        tileSys->addDirtyBounds(bounds, 1);
        etSceneSys->bvh->updateItemBoundsByObjectId(etId, bounds);
    });
}
void EntitySystemLayer::initalize(const std::string& configFileName)
{
    etSceneSys->initalize(configFileName);
    etRenderSys->entityStorage = etSceneSys->entityStorage;
    etRenderSys->initalize();
    tileSys->initalize();

    auto& etCompStorage = etSceneSys->entityStorage->comp;

    etSceneSys->interSrcSys->entityDirtyCall = [&, this](uint32_t type, const Base::ID::KeyUint64& etId) {
        if (type == 0)
        {
            updateTileWithEntityId(etId);
        }
        else
        {
            updateBVHAndTileWithEntityId(etId);
        }
    };
    etSceneSys->actionSys->entityDirtyCall = etSceneSys->interSrcSys->entityDirtyCall;

    uiOpLayer                      = std::make_shared<System::UIOperationLayer>();
    uiOpLayer->mouseCtrl.dirtyCall = [&, this](const Math::Bounds& bounds, uint32_t type, const Base::ID::KeyUint64& etId) {
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
    auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) -> int {
        auto&& ids = etSceneSys->getQueriedEIds();
        auto count = etRenderSys->render(drawCtx, vpMat, bounds, ids);
        return count;
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
        printf("Ctrl + Shift + Y pressed\n");
    });
    uiOpLayer->shortcutMana.registerShortcut({GLFW_KEY_DELETE}, [&, this] {
        auto et = uiOpLayer->mouseCtrl.selectEtId;
        printf("Press Delete Key et: %s\n", et.idToString().c_str());
        auto&& bv = etCompStorage->getEntityGlobalBoundsAt(et);
        tileSys->addDirtyBounds(bv, 0);
        etSceneSys->bvh->removeItemByObjectId(et);
    });
}


void EntitySystemLayer::updateCtx(const Render::Draw::DrawContext& ctx)
{

    drawCtx.clearParam                    = ctx.clearParam;
    drawCtx.drawParam                     = ctx.drawParam;
    drawCtx.fboGraph.backgroundClearParam = drawCtx.clearParam;
}

void EntitySystemLayer::undo()
{
    auto&  storage     = etRenderSys->entityStorage;
    auto&  compStorage = storage->comp;
    auto&& itemData    = compStorage->historyManager->popItem();
    //printf("EntitySystemLayer::undo() itemData.id: %u\n", itemData.id.id());

    if (itemData.id.isIDInvalid())
    {
        return;
    }

    //printf("EntitySystemLayer::undo() update some items.\n");
    auto&& etrans = compStorage->getEntityTransformAt(itemData.id.id());

    Math::Vec2 pv{itemData.trans.x, itemData.trans.y};

    updateTileWithEntityId(itemData.id);
    compStorage->setEntityLocalXYAt(pv, itemData.id.id());
    updateBVHAndTileWithEntityId(itemData.id);
}

void EntitySystemLayer::render(const Math::Mat33& vpMat)
{
    etSceneSys->update();
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