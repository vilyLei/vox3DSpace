#ifndef VOXOL_RENDER_ENTITY_COMP_H
#define VOXOL_RENDER_ENTITY_COMP_H

#include "../Math/VxRect.h"
#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace Voxol::Render
{

namespace Component
{
constexpr uint32_t INVALID_ID = 0xffffffff;

struct UnitTransform
{
    float x;
    float y;
    /// x-axis scale
    float sx;
    /// y-axis scale
    float sy;
    ///  rotation radian
    float rotation;

    inline Math::Vec2&       pos() { return *reinterpret_cast<Math::Vec2*>(&x); }
    inline Math::Vec2&       scale() { return *reinterpret_cast<Math::Vec2*>(&sx); }
    inline const Math::Vec2& pos() const { return *reinterpret_cast<const Math::Vec2*>(&x); }
    inline const Math::Vec2& scale() const { return *reinterpret_cast<const Math::Vec2*>(&sx); }
};

struct UnitShadingBaseDesc
{
    uint32_t color = 0xff000000;
    uint32_t type  = 0x0;
};

struct UnitShadingEntity
{
    uint32_t id = -1;
    /// UnitShadingBaseDesc vector index value
    uint32_t shadingDescId = -1;
};
struct UnitModel
{
    uint32_t id         = -1;
    uint32_t drawUnitId = -1;
};

struct UnitHierarchy
{
    uint32_t parent;
    uint32_t next;
    uint32_t firstChild;
};
struct UnitEntity
{
    ///  只有entity才有自身的id
    uint32_t id = -1;
    /// UnitShadingEntity vector index value
    uint32_t shadingId   = -1;
    uint32_t transformId = -1;
    uint32_t modelId     = -1;
    uint32_t hierarchyId = -1;
    // 表示这个 entity是基于某个entity的instance(实例)
    uint32_t prototype = -1;

    bool visible = true;
    bool dirty   = true;
};

} // namespace Component

} // namespace Voxol::Render
#endif