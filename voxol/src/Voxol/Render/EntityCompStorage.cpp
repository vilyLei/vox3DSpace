#include "EntityCompStorage.h"
#include <algorithm>

namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(uint32_t id)
{

    auto& et = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{

    auto& et                       = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}

Math::Vec2 EntityCompStorage::getEntityXYAt(uint32_t id)
{
    auto&  et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}
void EntityCompStorage::setEntityXYAt(const Math::Vec2& pos, uint32_t id)
{
    auto&  et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    trans.x      = pos.x;
    trans.y      = pos.y;
}

void EntityCompStorage::checkIds(std::vector<uint32_t>& edis)
{
    if (edis.empty())
    {
        return;
    }
    std::vector<uint32_t> ids{};
    auto                  tot = edis.size();
    for (auto i = 0; i < tot; ++i)
    {
        auto&& et = entitiesPool[edis[i]];
        if (!et.visible)
            continue;
        ids.push_back(edis[i]);
    }
    if (edis.size() != ids.size())
    {
        edis = ids;
    }
    if (edis.size() <= 1)
    {
        return;
    }
    std::sort(edis.begin(), edis.end(), [&](uint32_t a, uint32_t b) {
        //return entitiesPool[a].id < entitiesPool[b].id;
        return a < b;
    });
}

void EntityCompStorage::checkWMaps(std::vector<uint32_t>& edis)
{

}
} // namespace Voxol::Render