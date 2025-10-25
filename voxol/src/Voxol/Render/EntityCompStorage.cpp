#include "EntityCompStorage.h"
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
} // namespace Voxol::Render