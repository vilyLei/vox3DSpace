#include "InteractionSourceSystem.h"
namespace Voxol::Intent
{
InteractionSourceSystem::SP InteractionSourceSystem::make()
{
    auto sp = std::make_shared<InteractionSourceSystem>();
    return sp;
}
void InteractionSourceSystem::initialize()
{
}
}