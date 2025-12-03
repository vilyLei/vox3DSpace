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

}
void ActionSystem::update(){


    std::vector<Scene::EntityMotionObject::SP> objs;
    motionObjStorage->foreachObjs([&](const Scene::EntityMotionObject::SP& obj) -> bool {
        objs.emplace_back(obj);
        return true;
    });
    auto total = objs.size();
    if (total > 0)
    {

        auto&& key = Base::ID::KeyUint64::make(objs[0]->etId());
        entityDirtyCall(0, key);
        objs[0]->update();
        objs[0]->applyPoints([&](int index, const Math::Vec2& pv0, const Math::Vec2& pv1) -> bool {

            auto i = index + 1;

            auto dv = pv1 - pv0;

            auto&& key = Base::ID::KeyUint64::make(objs[i]->etId());
            entityDirtyCall(0, key);
            objs[i]->globalPos( pv0 );
            objs[i]->rotation( dv.radian() );
            return true;
        });
    }

    auto& dirtyMap = compStorage->dirtyIdMap;
    if (!dirtyMap.empty())
    {
        for (auto& item : dirtyMap)
        {
            auto&& key = Base::ID::KeyUint64::make(item.first);
            entityDirtyCall(1, key);
        }
        dirtyMap.clear();
    }
}

} // namespace Voxol::Intent