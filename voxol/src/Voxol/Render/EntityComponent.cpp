#include "EntityComponent.h"

namespace Voxol::Render
{

namespace Component
{

uint32_t UnitInstanceIdManager::allocate()
{
    if (!freeList.empty())
    {
        auto id = freeList.back();
        freeList.pop_back();
        return id;
    }
    return nextId++;
}

void UnitInstanceIdManager::release(uint32_t id)
{
    freeList.push_back(id);
}
} // namespace Component

} // namespace Voxol::Render