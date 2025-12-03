#ifndef VOXOL_SCENE_ENTITY_MOTION_OBJECT_H
#define VOXOL_SCENE_ENTITY_MOTION_OBJECT_H

#include "EntityCompStorage.h"
#include <deque>

namespace Voxol::Scene
{
using UpdateBVHCallbackType       = std::function<void(uint32_t id)>;
using UpdateTransformCallbackType = std::function<bool(const Math::Vec2& wPos, Component::UnitTransform& trans)>;
using PtApplyCallbackType         = std::function<bool(int index, const Math::Vec2& pv0, const Math::Vec2& pv1)>;

class EntityMotionObject
{
public:
    using SP = std::shared_ptr<EntityMotionObject>;
    static SP make();

public:
    EntityMotionObject()  = default;
    ~EntityMotionObject() = default;

public:
    UpdateBVHCallbackType  bvhUpdateCall;
    Math::Vec2             targetPos{500, 300};
    std::deque<Math::Vec2> pts;


public:
    void       initialize(uint32_t etId, EntityCompStorage::SP comp_storage);
    bool       isValid() const;
    bool       isInvalid() const;
    void       color(uint32_t c);
    uint32_t   color() const;
    void       localPos(const Math::Vec2& pos);
    Math::Vec2 localPos() const;
    void       globalPos(const Math::Vec2& pos);
    Math::Vec2 globalPos() const;
    void       rotation(float rad);
    float      rotation() const;
    void       rotationDeegree(float degree);
    float      rotationDeegree() const;

    uint32_t etProtoId() const;
    void     update();
    void     applyPoints(PtApplyCallbackType callback);
    void     destory();

private:
    EntityCompStorage::SP compStorage;
    uint32_t            targetEtProtoId = Base::ID::INVALID_ID;
};
} // namespace Voxol::Scene
#endif