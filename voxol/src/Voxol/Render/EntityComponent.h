#ifndef VOXOL_RENDER_ENTITY_COMPONENT_H
#define VOXOL_RENDER_ENTITY_COMPONENT_H

#include "../Math/VxRect.h"
#include "CompBaseDef.h"


namespace Voxol::Render
{
namespace Component
{
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


struct UnitEffectShadow
{
    uint32_t   color = 0xff000000;
    Math::Vec2 offset;
    float      blurRadius = 0;
};

struct UnitShadingDesc
{
    uint32_t color = 0xff000000;
    uint32_t flags  = 0x0;
};

struct UnitShadingEntity
{
    uint32_t id            = ID::INVALID_ID;
    uint32_t shadingDescId = ID::INVALID_ID;
};
struct UnitModel
{
    uint32_t id         = ID::INVALID_ID;
    uint32_t drawUnitId = ID::INVALID_ID;
};

struct UnitHierarchy
{
    uint32_t parent     = ID::INVALID_ID;
    uint32_t next       = ID::INVALID_ID;
    uint32_t firstChild = ID::INVALID_ID;
};
struct UnitEntity
{
    uint32_t id = ID::INVALID_ID;
    /// UnitShadingEntity vector index value
    uint32_t shadingId   = ID::INVALID_ID;
    uint32_t transformId = ID::INVALID_ID;
    uint32_t modelId     = ID::INVALID_ID;
    uint32_t hierarchyId = ID::INVALID_ID;
    uint32_t prototypeId = ID::INVALID_ID;

    bool visible = true;
    bool dirty   = true;
};



struct InsNodeSlot
{
    uint32_t    id;
    Math::Mat33 mat;
    bool        active;
};

struct FlatInsStorage
{
    std::vector<InsNodeSlot>               nodes;
    std::unordered_map<uint32_t, uint32_t> idToIndex;
    std::vector<uint32_t>                  freeList;

    InsNodeSlot& emplace(uint32_t id, const Math::Mat33& mat)
    {
        if (!freeList.empty())
        {
            auto idx = freeList.back();
            freeList.pop_back();
            nodes[idx]    = {id, mat, true};
            idToIndex[id] = idx;
            return nodes[idx];
        }
        uint32_t idx = nodes.size();
        nodes.push_back({id, mat, true});
        idToIndex[id] = idx;
        return nodes.back();
    }

    void erase(uint32_t id)
    {
        auto it = idToIndex.find(id);
        if (it != idToIndex.end())
        {
            uint32_t idx      = it->second;
            nodes[idx].active = false;
            freeList.push_back(idx);
            idToIndex.erase(it);
        }
    }

    InsNodeSlot* find(uint32_t id)
    {
        auto it = idToIndex.find(id);
        return it != idToIndex.end() ? &nodes[it->second] : nullptr;
    }
};


struct UnitInstanceMap
{
    uint32_t                                  iid     = ID::INVALID_ID;
    uint32_t                                  protoId = ID::INVALID_ID;
    ID::keyUint64Unordered_map<ID::KeyUint64> map;
    bool                                      dirty = true;
};

} // namespace Component

} // namespace Voxol::Render
#endif