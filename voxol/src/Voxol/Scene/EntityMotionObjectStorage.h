#ifndef VOXOL_SCENE_ENTITY_MOTION_OBJECT_STORAGE_H
#define VOXOL_SCENE_ENTITY_MOTION_OBJECT_STORAGE_H

#include "EntityMotionObject.h"
#include <unordered_map>

namespace Voxol::Scene
{
using FroreachObjCallbackType = std::function<bool(const EntityMotionObject::SP& obj)>;

class EntityMotionObjectStorage
{
public:
    using SP = std::shared_ptr<EntityMotionObjectStorage>;
    static SP make();

public:
    EntityMotionObjectStorage()  = default;
    ~EntityMotionObjectStorage() = default;

public:
    EntityMotionObject::SP        mainObject;
    EntityMotionObjectStorage::SP foodStorage;

public:
    void initialize();
    void updateAction(const Scene::Component::UnitLocation& location);
    void addObject(const EntityMotionObject::SP& obj);
    void setObject(uint32_t protoId, const EntityMotionObject::SP& obj);
    void update();
    void foreachObjs(FroreachObjCallbackType callback);
    void destory();
    void clear();

private:
    std::unordered_map<uint32_t, EntityMotionObject::SP> objsMap;
};
} // namespace Voxol::Scene
#endif