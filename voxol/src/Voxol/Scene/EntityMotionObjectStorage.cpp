
#include "EntityMotionObjectStorage.h"

namespace Voxol::Scene
{

EntityMotionObjectStorage::SP EntityMotionObjectStorage::make()
{
    auto sp = std::make_shared<EntityMotionObjectStorage>();
    return sp;
}

void EntityMotionObjectStorage::initialize()
{
}

void EntityMotionObjectStorage::updateAction(const Scene::Component::UnitLocation& location)
{
    if (mainObject)
    {

        mainObject->targetPos = location.global;
    }
}
void EntityMotionObjectStorage::addObject(const EntityMotionObject::SP& obj)
{

    if (!obj || obj->isInvalid() || objsMap.contains(obj->etProtoId()))
        return;

    objsMap[obj->etProtoId()] = obj;
}

void EntityMotionObjectStorage::setObject(uint32_t protoId, const EntityMotionObject::SP& obj)
{

    if (Base::ID::isInvalidID(protoId))
        return;

    
    objsMap[protoId] = obj;

}

void EntityMotionObjectStorage::update()
{

    if (!mainObject)
        return;

    std::vector<Scene::EntityMotionObject::SP> objs;
    objs.reserve(32);

    auto currPos = mainObject->entityView->globalPos();
    if (foodStorage)
    {

        foodStorage->foreachObjs([&](const Scene::EntityMotionObject::SP& obj) -> bool {
            objs.emplace_back(obj);
            return true;
        });

        for (auto& obj : objs) {
            auto&& pos = obj->entityView->globalPos();
            auto   dv  = pos - currPos;
            if (dv.length() < 10)
            {
                printf("........... hit ....fdfd\n");
            }

        }
        objs.clear();
    }
    foreachObjs([&](const Scene::EntityMotionObject::SP& obj) -> bool {
        objs.emplace_back(obj);
        return true;
    });


    mainObject->update();
    auto total = objs.size();
    if (mainObject && total > 0)
    {
        mainObject->applyPoints([&](int index, const Math::Vec2& pv0, const Math::Vec2& pv1) -> bool {

            auto i = index;
            if (i >= total)
                return false;

            auto dv = pv1 - pv0;

            auto   etView = objs[i]->entityView;

            etView->globalPos(pv0);
            etView->rotation(dv.radian());
            return true;
        });
    }
}

void EntityMotionObjectStorage::foreachObjs(FroreachObjCallbackType callback)
{

    for (auto& item : objsMap)
    {

        auto& sp = item.second;
        if (!sp)
            break;

        auto flag = callback(sp);
        if (!flag)
            break;
    }
}
void EntityMotionObjectStorage::destory()
{
    objsMap.clear();
    foodStorage = nullptr;
}

void EntityMotionObjectStorage::clear()
{
    objsMap.clear();
    foodStorage = nullptr;
}
} // namespace Voxol::Scene