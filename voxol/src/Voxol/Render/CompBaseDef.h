#ifndef VOXOL_RENDER_COMP_BASE_DEF_H
#define VOXOL_RENDER_COMP_BASE_DEF_H

#include "../Math/VxRect.h"
#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <unordered_map>
#include <string>

namespace Voxol::Render
{
namespace Base
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

struct EntityIdManager
{
    uint32_t              nextId = 1; // 0 reserved/invalid
    std::vector<uint32_t> freeList;

    uint32_t allocate();
    void     release(uint32_t id);
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

struct KeyUint64
{
    uint64_t value;

    static constexpr uint64_t ProtoMask   = (1ull << 24) - 1; // 24-bit
    static constexpr uint64_t IIDMask     = ProtoMask << 24;  // 24-bit << 24
    static constexpr uint64_t FlagMask    = 0xFFFFull << 48;  // 16-bit << 48
    static constexpr uint64_t CompareMask = (1ull << 48) - 1; // lower 48 bits

    static constexpr KeyUint64 make(uint16_t flags, uint32_t protoId, uint32_t iid)
    {
        return KeyUint64{(uint64_t(flags) << 48) | (uint64_t(iid) << 24) | uint64_t(protoId)};
    }

    constexpr uint16_t flags() const noexcept { return value >> 48; }
    constexpr uint32_t protoNodeId() const noexcept { return value & ProtoMask; }
    constexpr uint32_t iid() const noexcept { return (value >> 24) & ProtoMask; }

    constexpr bool operator==(const KeyUint64& other) const noexcept
    {
        return (value & CompareMask) == (other.value & CompareMask);
    }

    constexpr bool operator<(const KeyUint64& other) const noexcept
    {
        return (value & CompareMask) < (other.value & CompareMask);
    }
    std::string                toString(bool hex = false) const;
    constexpr std::string_view debugView() const noexcept
    {
        return "<Component::KeyUint64>";
    }
};

struct KeyUint64Hasher
{
    size_t operator()(const KeyUint64& k) const noexcept
    {
        return std::hash<uint64_t>()(k.value & KeyUint64::CompareMask);
    }
};

struct KeyUint64Equal
{
    bool operator()(const KeyUint64& a, const KeyUint64& b) const noexcept
    {
        return (KeyUint64::ProtoMask & a.value) == (KeyUint64::ProtoMask & b.value);
    }
};
} // namespace Component

} // namespace Voxol::Render
#endif