#include "EntityIDFactory.h"
#include <queue>
namespace Voxol::Base
{
/*
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

void EntityIDFactory::remove(VoxolEntity id)
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
//*/

class EntityIDFactory::Impl
{
public:
    std::vector<uint8_t>    versions;
    std::queue<VoxolEntity> freeList;

    VoxolEntity create()
    {
        if (!freeList.empty())
        {
            VoxolEntity index = freeList.front();
            freeList.pop();
            return makeEntity(index, versions[index]);
        }
        VoxolEntity index = static_cast<VoxolEntity>(versions.size());
        versions.push_back(0);
        return makeEntity(index, 0);
    }

    void remove(VoxolEntity entity)
    {
        VoxolEntity index = extractIndex(entity);
        if (index >= versions.size()) return;

        ++versions[index]; // version flag
        freeList.push(index);
    }

    uint8_t version(VoxolEntity entity) const
    {
        return versions[extractIndex(entity)];
    }

    bool isAlive(VoxolEntity entity) const
    {
        VoxolEntity index = extractIndex(entity);
        uint8_t     v     = extractVersion(entity);
        return index < versions.size() && versions[index] == v;
    }
    void reset()
    {
        freeList = {};
        versions.clear();
    }
};

// 实现外部接口

EntityIDFactory::EntityIDFactory() :
    impl(new Impl()) {}

VoxolEntity EntityIDFactory::create(const std::string& name)
{
    auto id = impl->create();
    if (!name.empty())
    {
        entityNames[id]    = name;
        nameToEntity[name] = id;
    }

    printf("EntityIDFactory::create() eneity id: %d\n", id);
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

void EntityIDFactory::remove(VoxolEntity entity)
{
    impl->remove(entity);
}

bool EntityIDFactory::isAlive(VoxolEntity entity) const
{
    return impl->isAlive(entity);
}

uint8_t EntityIDFactory::version(VoxolEntity entity) const
{
    return impl->version(entity);
}

VoxolEntity EntityIDFactory::makeEntity(VoxolEntity index, uint8_t version)
{
    return (static_cast<VoxolEntity>(version) << indexBits) | index;
}

VoxolEntity EntityIDFactory::extractIndex(VoxolEntity entity)
{
    return entity & indexMask;
}

uint8_t EntityIDFactory::extractVersion(VoxolEntity entity)
{
    return static_cast<uint8_t>((entity >> indexBits) & versionMask);
}
void EntityIDFactory::reset()
{
    impl->reset();
}


} // namespace Voxol::Base