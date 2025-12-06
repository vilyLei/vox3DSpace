#include "ActionSystem.h"
namespace Voxol::Intent
{

ActionSystem::SP ActionSystem::make()
{
    auto sp = std::make_shared<ActionSystem>();
    return sp;
}

void ActionSystem::initialize() {

}
void ActionSystem::updateAction(const Scene::Component::UnitLocation& location){

    positions.emplace_back(location.global);
    if (aimEtView)
    {
        auto&& key = Base::ID::KeyUint64::make(aimEtView->etProtoId());
        entityDirtyCall(0, key);
        aimEtView->globalPos(location.global);
    }
}
void ActionSystem::update(){

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

    std::vector<Scene::EntityMotionObject::SP> objs;
    motionObjStorage->foreachObjs([&](const Scene::EntityMotionObject::SP& obj) -> bool {
        objs.emplace_back(obj);
        return true;
    });
    auto total = objs.size();
    if (total > 0)
    {
        if (!positions.empty())
        {
            objs[0]->targetPos = positions[0];
        }
        objs[0]->update();
        objs[0]->applyPoints([&](int index, const Math::Vec2& pv0, const Math::Vec2& pv1) -> bool {

            auto i = index + 1;
            if (i >= total)
            {
                return false;
            }
            auto dv = pv1 - pv0;

            auto&& key = Base::ID::KeyUint64::make(objs[i]->etProtoId());
            auto etView = objs[i]->entityView;
            etView->globalPos(pv0);
            etView->rotation(dv.radian());
            return true;
        });
    }
    positions.clear();

    auto& dirtyEndMap = compStorage->dirtyEntityMap;
    if (!dirtyEndMap.empty())
    {
        for (auto& item : dirtyEndMap)
        {
            auto&& key = Base::ID::KeyUint64::make(item.first);
            entityDirtyCall(0, key);
            entityDirtyCall(1, key);
        }
        dirtyEndMap.clear();
    }
}

} // namespace Voxol::Intent