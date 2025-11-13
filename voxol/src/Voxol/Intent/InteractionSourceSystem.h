#ifndef VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H
#define VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H

#include "../Base/BaseDefine.h"
#include "../Render/CompBaseDef.h"

namespace Voxol::Intent
{
namespace Interaction
{
struct MouseSrcNode
{
    Render::ID::KeyUint64 id;
    Render::ID::KeyUint64 dstID;
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