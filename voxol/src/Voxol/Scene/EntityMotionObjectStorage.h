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
public:
    void initialize();
    void addObject(const EntityMotionObject::SP& obj);
    void update();
    void foreachObjs(FroreachObjCallbackType callback);
    void destory();

private:
    std::unordered_map<uint32_t, EntityMotionObject::SP> objsMap;
};
} // namespace Voxol::Scene
#endif