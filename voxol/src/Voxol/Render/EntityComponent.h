#ifndef VOXOL_RENDER_ENTITY_COMPONENT_H
#define VOXOL_RENDER_ENTITY_COMPONENT_H

#include "../Math/VxRect.h"
#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <unordered_map>

namespace Voxol::Render
{

namespace Component
{
constexpr uint32_t INVALID_ID = 0xffffffff;

constexpr inline bool isValidID(uint32_t id) noexcept
{
    return id != INVALID_ID;
}
constexpr inline bool isInvalidID(uint32_t id) noexcept
{
    return id == INVALID_ID;
}
struct EntityId
{
    uint32_t value = INVALID_ID;

    constexpr bool valid() const noexcept { return value != INVALID_ID; }

    constexpr bool operator==(const EntityId& other) const noexcept
    {
        return value == other.value;
    }
};
struct EntityIdHasher
{
    size_t operator()(const EntityId& id) const noexcept
    {
        return std::hash<uint32_t>()(id.value);
    }
};
struct EntityIdEqual
{
    bool operator()(const EntityId& a, const EntityId& b) const noexcept
    {
        return a.value == b.value;
    }
};

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
constexpr UnitTransform defaultTrans{0, 0, 1, 1, 0};
constexpr Math::VxRect  defaultRect{0, 0, 1, 1};

struct UnitMat33
{
    Math::Mat33 mat;
    uint32_t    id = INVALID_ID;
};
struct UnitShadingBaseDesc
{
    uint32_t color = 0xff000000;
    uint32_t type  = 0x0;
};

struct UnitShadingEntity
{
    uint32_t id = INVALID_ID;
    /// UnitShadingBaseDesc vector index value
    uint32_t shadingDescId = INVALID_ID;
};
struct UnitModel
{
    uint32_t id         = INVALID_ID;
    uint32_t drawUnitId = INVALID_ID;
};

struct UnitHierarchy
{
    uint32_t parent;
    uint32_t next;
    uint32_t firstChild;
};
struct UnitEntity
{
    /// 只有entity才有自身的id
    uint32_t id = INVALID_ID;
    /// UnitShadingEntity vector index value
    uint32_t shadingId   = INVALID_ID;
    uint32_t transformId = INVALID_ID;
    uint32_t modelId     = INVALID_ID;
    uint32_t hierarchyId = INVALID_ID;
    // 表示当前这个entity是基于prototypeId所对应的entity的instance(实例)
    uint32_t prototypeId = INVALID_ID;

    bool visible = true;
    bool dirty   = true;
};

struct UnitInstance
{
    uint32_t    protoNodeId = INVALID_ID; // prototype entity id
    uint32_t    iid         = INVALID_ID; // unique instantiation id
    Math::Mat33 worldMat;
};
struct UnitInstanceMap
{
    uint32_t                             instanceEntityId = INVALID_ID; // 场景中的 instance 根实体
    uint32_t                             prototypeRootId  = INVALID_ID; // 源 prototype 根 id
    std::vector<UnitInstance>            nodes;                         // 扁平列表，或 unordered_map<iid, UnitInstance>
    std::unordered_map<uint32_t, size_t> iidToIndexMap;                 // iid -> index in nodes (optional)
    bool                                 dirty = true;
};
struct UnitInstanceIdManager
{
    uint32_t              nextId = 1; // 0 reserved/invalid
    std::vector<uint32_t> freeList;

    uint32_t alloc();
    void free(uint32_t id);
};
} // namespace Component

} // namespace Voxol::Render
#endif