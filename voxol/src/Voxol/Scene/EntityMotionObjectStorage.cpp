
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

void EntityMotionObjectStorage::addObject(const EntityMotionObject::SP& obj)
{

    if (!obj || obj->isInvalid() || objsMap.contains(obj->etId()))
        return;

    objsMap[obj->etId()] = obj;
}
void EntityMotionObjectStorage::update(){
}

void EntityMotionObjectStorage::foreachObjs(FroreachObjCallbackType callback)
{

    for (auto& item : objsMap) {

        auto flag = callback(item.second);
        if (!flag)
            break;
    }
}
void EntityMotionObjectStorage::destory(){
    objsMap.clear();
}
} // namespace Voxol::Scene