#ifndef VOXOL_INTENT_ACTION_SYSTEM_H
#define VOXOL_INTENT_ACTION_SYSTEM_H

#include "IntentComponent.h"
#include "../Scene/EntityCompStorage.h"
#include "../Scene/EntityMotionObjectStorage.h"
#include <functional>

namespace Voxol::Intent
{

class ActionSystem
{
public:
    using SP = std::shared_ptr<ActionSystem>;
    using WP = std::weak_ptr<ActionSystem>;
    using UP = std::unique_ptr<ActionSystem>;

public:
    static ActionSystem::SP make();

public:
    ActionSystem()  = default;
    ~ActionSystem() = default;

public:
    Scene::EntityCompStorage::SP         compStorage;
    Scene::EntityMotionObjectStorage::SP motionObjStorage;
    Interaction::EntityDirtyCallType     entityDirtyCall;

public:
    void initialize();
    void updateAction(const Scene::Component::UnitLocation& location);
    void update();

private:
    std::vector<Math::Vec2> positions;
};


} // namespace Voxol::Intent
#endif