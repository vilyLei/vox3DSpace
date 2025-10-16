#include "EntityCompStorage.h"
namespace Voxol::Render
{
    
    Math::Vec2 EntityCompStorage::getEntityXYAt(int32_t id)
    {
        auto& et        = entities[id];
        auto& shadingEt = get<Component::UnitShadingEntity>(et.id);
        auto& shdDesc   = get<Component::UnitShadingBaseDesc>(shadingEt.shadingDescId);
        auto& trans     = shdDesc.transform;
        return {trans.x, trans.y};
    }
    void EntityCompStorage::setEntityXYAt(const Math::Vec2& pos, int32_t id)
    {
        auto& et        = entities[id];
        auto& shadingEt = get<Component::UnitShadingEntity>(et.id);
        auto& shdDesc   = get<Component::UnitShadingBaseDesc>(shadingEt.shadingDescId);
        auto& trans     = shdDesc.transform;
        trans.x         = pos.x;
        trans.y         = pos.y;
    }
}