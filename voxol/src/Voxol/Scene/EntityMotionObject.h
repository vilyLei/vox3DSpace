#ifndef VOXOL_SCENE_ENTITY_MOTION_OBJECT_H
#define VOXOL_SCENE_ENTITY_MOTION_OBJECT_H

#include "EntityCompStorage.h"
#include "EntityView.h"
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
    EntityView::SP             entityView;
    UpdateBVHCallbackType  bvhUpdateCall;
    Math::Vec2             targetPos{500, 300};
    std::deque<Math::Vec2> pts;


public:
    void                     initialize(uint32_t etId, EntityCompStorage::SP comp_storage);
    bool                     isValid() const;
    bool                     isInvalid() const;

    uint32_t etProtoId() const;
    void     update();
    void     applyPoints(PtApplyCallbackType callback, int stride = 20);
    void     destory();

private:
    //EntityCompStorage::SP compStorage;
    //uint32_t              targetEtProtoId = Base::ID::INVALID_ID;
};
} // namespace Voxol::Scene
#endif