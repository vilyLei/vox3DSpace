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

        auto&& key = Base::ID::KeyUint64::make(objs[0]->etProtoId());
        entityDirtyCall(0, key);
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
            entityDirtyCall(0, key);
            objs[i]->globalPos( pv0 );
            objs[i]->rotation( dv.radian() );
            return true;
        });
    }
    positions.clear();

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