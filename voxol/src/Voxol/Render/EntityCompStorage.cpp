#include "EntityCompStorage.h"
#include <algorithm>

namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(int32_t id) {

    auto& et        = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, int32_t id) {

    auto& et        = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}

Math::Vec2 EntityCompStorage::getEntityXYAt(int32_t id)
{
    auto&  et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}
void EntityCompStorage::setEntityXYAt(const Math::Vec2& pos, int32_t id)
{
    auto& et        = entitiesPool[id];
    auto&& trans     = transformsPool[et.transformId];
    trans.x         = pos.x;
    trans.y         = pos.y;
}

void EntityCompStorage::checkIds(std::vector<int32_t>& edis)
{
    if (edis.empty())
    {
        return;
    }
    std::vector<int32_t> ids{};
    auto                 tot = edis.size();
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
    if (edis.size() > 1)
    {
        std::sort(edis.begin(), edis.end(), [&](int32_t a, int32_t b) {
            return a < b;
        });
    }
}
} // namespace Voxol::Render