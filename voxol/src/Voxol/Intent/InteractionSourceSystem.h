#ifndef VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H
#define VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H

#include "../Base/BaseDefine.h"

namespace Voxol::Intent
{
class InteractionSourceSystem
{
public:
    using SP = std::shared_ptr<InteractionSourceSystem>;
    using WP = std::weak_ptr<InteractionSourceSystem>;
    using UP = std::unique_ptr<InteractionSourceSystem>;

public:
    static InteractionSourceSystem::SP make();

public:
    InteractionSourceSystem() = default;
    ~InteractionSourceSystem() = default;

public:
    void initialize();

private:
};


}
#endif