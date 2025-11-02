#ifndef VOXOL_RENDER_ENTITY_COMPONENT_H
#define VOXOL_RENDER_ENTITY_COMPONENT_H

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

struct UnitMat33
{
    Math::Mat33 mat;
    uint32_t    id = Base::INVALID_ID;
};
struct UnitShadingBaseDesc
{
    uint32_t color = 0xff000000;
    uint32_t type  = 0x0;
};

struct UnitShadingEntity
{
    uint32_t id = Base::INVALID_ID;
    /// UnitShadingBaseDesc vector index value
    uint32_t shadingDescId = Base::INVALID_ID;
};
struct UnitModel
{
    uint32_t id         = Base::INVALID_ID;
    uint32_t drawUnitId = Base::INVALID_ID;
};

struct UnitHierarchy
{
    uint32_t parent     = Base::INVALID_ID;
    uint32_t next       = Base::INVALID_ID;
    uint32_t firstChild = Base::INVALID_ID;
};
struct UnitEntity
{
    /// 只有entity才有自身的id
    uint32_t id = Base::INVALID_ID;
    /// UnitShadingEntity vector index value
    uint32_t shadingId   = Base::INVALID_ID;
    uint32_t transformId = Base::INVALID_ID;
    uint32_t modelId     = Base::INVALID_ID;
    uint32_t hierarchyId = Base::INVALID_ID;
    // 表示当前这个entity是基于prototypeId所对应的entity的instance(实例)
    uint32_t prototypeId = Base::INVALID_ID;

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


struct UnitInstance
{
    uint32_t    protoNodeId = Base::INVALID_ID; // prototype entity id
    uint32_t    iid         = Base::INVALID_ID; // unique instantiation id
    Math::Mat33 worldMat;
};

struct UnitInstanceMap
{
    uint32_t                             instanceEntityId = Base::INVALID_ID; // 场景中的 instance 根实体
    uint32_t                             prototypeRootId  = Base::INVALID_ID; // 源 prototype 根 id
    std::vector<UnitInstance>            nodes;                         // 扁平列表，或 unordered_map<iid, UnitInstance>
    std::unordered_map<uint32_t, size_t> iidToIndexMap;                 // iid -> index in nodes (optional)
    bool                                 dirty = true;
    // for debug
    std::unordered_map<uint32_t, UnitInstance> map;
};

} // namespace Component

} // namespace Voxol::Render
#endif