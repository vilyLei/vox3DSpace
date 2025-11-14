#ifndef VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H
#define VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H

#include "intentPreDef.h"
#include "EnumWrapper.h"
#include "../Render/CompBaseDef.h"

namespace Voxol::Intent
{
namespace Interaction
{
enum class MouseActionMode : uint8_t
{
    None  = 0,
    Out  = 1 << 0,
    Over = 1 << 1,
    Down   = 1 << 2,
    Up = 1 << 3,
    Click = 1 << 4,
    Move = 1 << 5
};
struct MouseSrcNode
{
    Render::ID::KeyUint64 id;
    Render::ID::KeyUint64 dstID;
    Flag::EnumWrapper<MouseActionMode> flags;

    std::string           action = "out";
};
} // namespace Interaction
class InteractionSourceSystem
{
public:
    using SP = std::shared_ptr<InteractionSourceSystem>;
    using WP = std::weak_ptr<InteractionSourceSystem>;
    using UP = std::unique_ptr<InteractionSourceSystem>;

public:
    static InteractionSourceSystem::SP make();

public:
    InteractionSourceSystem()  = default;
    ~InteractionSourceSystem() = default;

public:
    void initialize();

private:
};


} // namespace Voxol::Intent
#endif