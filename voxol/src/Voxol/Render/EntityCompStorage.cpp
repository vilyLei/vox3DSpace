#include "EntityCompStorage.h"
namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}


void EntityCompStorage::initialize(int total)
{

}

Math::Vec2 EntityCompStorage::getEntityXYAt(int32_t id)
{
    auto& et        = entitiesPool[id];
    auto& shadingEt = get<Component::UnitShadingEntity>(et.shadingId);
    auto& shdDesc   = get<Component::UnitShadingBaseDesc>(shadingEt.shadingDescId);
    auto& trans     = shdDesc.transform;
    return {trans.x, trans.y};
}
void EntityCompStorage::setEntityXYAt(const Math::Vec2& pos, int32_t id)
{
    auto& et        = entitiesPool[id];
    auto& shadingEt = get<Component::UnitShadingEntity>(et.shadingId);
    auto& shdDesc   = get<Component::UnitShadingBaseDesc>(shadingEt.shadingDescId);
    auto& trans     = shdDesc.transform;
    trans.x         = pos.x;
    trans.y         = pos.y;
}
} // namespace Voxol::Render