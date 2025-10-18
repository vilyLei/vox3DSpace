#ifndef VOXOL_RENDER_ENTITY_COMP_H
#define VOXOL_RENDER_ENTITY_COMP_H

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace Voxol::Render
{
    
namespace Component
{

struct UnitTransform
{
    float x = 0;
    float y = 0;

    /// x-axis scale
    float sx = 1;
    /// y-axis scale
    float sy = 1;

    ///  rotation radian
    float rotation = 0;
};

struct UnitShadingBaseDesc
{
    UnitTransform transform{};
    uint32_t      color = 0xff000000;
};

struct UnitShadingEntity
{
    ///  只有entity才有自身的id
    int32_t id = -1;
    /// UnitShadingBaseDesc vector index value
    int32_t shadingDescId = -1;
    /// DrawingUnit vector index value
    int32_t drawUnitId = -1;
};

struct UnitEntity
{
    ///  只有entity才有自身的id
    int32_t id = -1;
    /// UnitShadingEntity vector index value
    int32_t shadingId = -1;
    bool    visible   = true;
    bool    dirty   = true;
};

} // namespace Component

}
#endif