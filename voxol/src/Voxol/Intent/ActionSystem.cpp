#include "ActionSystem.h"
namespace Voxol::Intent
{

ActionSystem::SP ActionSystem::make()
{
    auto sp = std::make_shared<ActionSystem>();
    return sp;
}


void ActionSystem::initialize()
{
}

void ActionSystem::addMotionObjStorage(const Scene::EntityMotionObjectStorage::SP& motionObjStorage)
{
    if (!motionObjStorage)
        return;
    motionObjStorages.emplace_back(motionObjStorage);
}

void ActionSystem::updateAction(const Scene::Component::UnitLocation& location)
{

    if (aimEtView)
    {
        auto&& key = Base::ID::KeyUint64::make(aimEtView->etProtoId());
        entityDirtyCall(0, key);
        aimEtView->globalPos(location.global);
    }

    for (auto& storage : motionObjStorages)
    {
        storage->updateAction(location);
    }
}
void ActionSystem::update()
{

    if (!aimEtView)
    {
        aimEtView = Scene::EntityView::make();
        aimEtView->initializeWithName("aim-cross-sprite", compStorage);
    }
    if (aimEtView)
    {
        static float alphaTime = 0;
        alphaTime += 0.01;
        aimEtView->colorAlpha(0.3f + 0.7f * std::abs(std::cos(alphaTime)));
        aimEtView->rotation(aimEtView->rotation() + 0.05f);
    }

    for (auto& storage : motionObjStorages)
    {
        storage->update();
    }

    auto& dirtyEndMap = compStorage->dirtyEntityMap;
    if (!dirtyEndMap.empty())
    {
        for (auto& item : dirtyEndMap)
        {

            auto& desc = item.second;
            entityDirtyCall(0, desc.key);

            int flag = 0;
            if (desc.isBoundsDirty())
            {
                flag = 1;
                entityDirtyCall(1, desc.key);
            }
            else
            {
                flag = 2;
            }
        }
        dirtyEndMap.clear();
    }
}

} // namespace Voxol::Intent