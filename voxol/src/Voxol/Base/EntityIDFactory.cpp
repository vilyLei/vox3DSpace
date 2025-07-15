#include "EntityIDFactory.h"

namespace Voxol::Base
{

VoxolEntity EntityIDFactory::create(const std::string& name)
{
    VoxolEntity id = nextId++;
    activeEntities.insert(id);
    if (!name.empty())
    {
        entityNames[id]    = name;
        nameToEntity[name] = id;
    }
    return id;
}

std::string EntityIDFactory::getName(VoxolEntity e) const
{
    auto it = entityNames.find(e);
    return it != entityNames.end() ? it->second : "<unnamed>";
}

VoxolEntity EntityIDFactory::getByName(const std::string& name) const
{
    auto it = nameToEntity.find(name);
    return it != nameToEntity.end() ? it->second : 0;
}

void EntityIDFactory::destroy(VoxolEntity id)
{
    activeEntities.erase(id);
    auto it = entityNames.find(id);
    if (it != entityNames.end())
    {
        nameToEntity.erase(it->second);
        entityNames.erase(it);
    }
}

bool EntityIDFactory::isAlive(VoxolEntity id) const { return activeEntities.count(id) > 0; }

void EntityIDFactory::reset()
{
    activeEntities.clear();
    entityNames.clear();
    nameToEntity.clear();
    nextId = 1;
}

} // namespace Voxol::Base