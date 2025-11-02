#ifndef VOXOL_RENDER_COMP_BASE_DEF_H
#define VOXOL_RENDER_COMP_BASE_DEF_H

#include "../Math/VxRect.h"
#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <unordered_map>
#include <string>
#include <format>

namespace Voxol::Render
{
namespace ID
{
// 定义entity id为28bits的uint32_t类型

// 28-bit invalid ID
constexpr uint32_t INVALID_ID = 0x0FFFFFFF;
constexpr uint32_t ID_BITS_COUNT = 28;
constexpr uint32_t ID2_ID_BITS_COUNT = 56; // 28 + 28

struct EntityId
{
    static constexpr uint32_t IDMask = INVALID_ID; // 28-bit
    uint32_t value = INVALID_ID;

    constexpr bool isValid() const noexcept { return value < INVALID_ID; }
    constexpr bool isInvalid() const noexcept { return value >= INVALID_ID; }
    constexpr uint32_t id() const noexcept { return value & IDMask; }

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

struct KeyUint64
{
    uint64_t value;

    static constexpr uint64_t ProtoMask   = (1ull << ID_BITS_COUNT) - 1; // 28-bit
    static constexpr uint64_t IIDMask     = ProtoMask << ID_BITS_COUNT; // 28-bit << 28
    static constexpr uint64_t FlagMask    = 0xFFFFull << ID2_ID_BITS_COUNT; // 8-bit << 56
    static constexpr uint64_t CompareMask = (1ull << ID2_ID_BITS_COUNT) - 1; // lower 56 bits

    static constexpr KeyUint64 makeDefault()
    {
        return KeyUint64{0};
    }
    static constexpr KeyUint64 make(EntityId protoId, EntityId iid, uint16_t flags = 0)
    {
        return KeyUint64{(uint64_t(flags) << ID2_ID_BITS_COUNT) | (uint64_t(iid.id()) << ID_BITS_COUNT) | uint64_t(protoId.id())};
    }
    static constexpr KeyUint64 make(uint32_t protoId, uint32_t iid, uint16_t flags = 0)
    {
        return KeyUint64{(uint64_t(flags) << ID2_ID_BITS_COUNT) | (uint64_t(iid) << ID_BITS_COUNT) | uint64_t(protoId)};
    }

    constexpr uint16_t flags() const noexcept { return value >> ID2_ID_BITS_COUNT; }
    constexpr uint32_t protoNodeId() const noexcept { return value & ProtoMask; }
    constexpr bool     isProtoNodeIdValid() const noexcept { return (value & ProtoMask) < INVALID_ID; }
    constexpr bool     isProtoNodeIdInvalid() const noexcept { return (value & ProtoMask) >= INVALID_ID; }

    constexpr uint32_t iid() const noexcept { return (value >> ID_BITS_COUNT) & ProtoMask; }
    constexpr bool     isIIDValid() const noexcept
    {
        return ((value >> ID_BITS_COUNT) & ProtoMask) < INVALID_ID;
    }
    constexpr bool isIIDInvalid() const noexcept
    {
        return ((value >> ID_BITS_COUNT) & ProtoMask) >= INVALID_ID;
    }
    constexpr uint32_t id() const noexcept { return value & CompareMask; }
    constexpr bool     isIDValid() const noexcept { return isProtoNodeIdValid() && isIIDValid(); }
    constexpr bool     isIDInvalid() const noexcept { return isProtoNodeIdInvalid() || isIIDInvalid(); }

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
        return (a.value & KeyUint64::CompareMask) == (b.value & KeyUint64::CompareMask);
    }
};

constexpr inline bool isValidID(uint32_t id) noexcept
{
    return id < INVALID_ID;
}
constexpr inline bool isInvalidID(uint32_t id) noexcept
{
    return id >= INVALID_ID;
}

constexpr inline bool isValidID(const KeyUint64& id) noexcept
{
    return id.isIDValid();
}
constexpr inline bool isInvalidID(const KeyUint64& id) noexcept
{
    return id.isIDInvalid();
}


// ============================================================
// current IdTraits template
// ============================================================
template <typename T>
struct IdTraits;

// ============================================================
// EntityId template specialization
// ============================================================
template <>
struct IdTraits<EntityId>
{
    using Type   = EntityId;
    using ValueT = uint32_t;

    static constexpr ValueT InvalidValue = INVALID_ID;

    static constexpr bool isValid(const EntityId& id) noexcept
    {
        return id.value < InvalidValue;
    }

    static constexpr bool isInvalid(const EntityId& id) noexcept
    {
        return id.value >= InvalidValue;
    }

    struct Hasher
    {
        size_t operator()(const EntityId& id) const noexcept
        {
            return std::hash<ValueT>()(id.value);
        }
    };

    struct Equal
    {
        bool operator()(const EntityId& a, const EntityId& b) const noexcept
        {
            return a.value == b.value;
        }
    };

    struct Less
    {
        bool operator()(const EntityId& a, const EntityId& b) const noexcept
        {
            return a.value < b.value;
        }
    };

    static std::string toString(const EntityId& id)
    {
        if (isValid(id))
            return std::format("EntityId({})", id.value);
        else
            return "EntityId(INVALID)";
    }
};

// ============================================================
// KeyUint64 template specialization
// ============================================================
template <>
struct IdTraits<KeyUint64>
{
    using Type   = KeyUint64;
    using ValueT = uint64_t;

    static constexpr bool isValid(const KeyUint64& k) noexcept
    {
        return k.isIDValid();
    }

    static constexpr bool isInvalid(const KeyUint64& k) noexcept
    {
        return k.isIDInvalid();
    }

    struct Hasher
    {
        size_t operator()(const KeyUint64& k) const noexcept
        {
            return std::hash<ValueT>()(k.value & KeyUint64::CompareMask);
        }
    };

    struct Equal
    {
        bool operator()(const KeyUint64& a, const KeyUint64& b) const noexcept
        {
            return (a.value & KeyUint64::CompareMask) == (b.value & KeyUint64::CompareMask);
        }
    };

    struct Less
    {
        bool operator()(const KeyUint64& a, const KeyUint64& b) const noexcept
        {
            return (a.value & KeyUint64::CompareMask) < (b.value & KeyUint64::CompareMask);
        }
    };

    static std::string toString(const KeyUint64& k)
    {
        return std::format("KeyUint64(flags={}, proto={}, iid={})",
                           k.flags(), k.protoNodeId(), k.iid());
    }
};

// ============================================================
// general template function defines
// ============================================================
template <typename T>
constexpr bool isValidID(const T& id) noexcept
{
    return IdTraits<T>::isValid(id);
}

template <typename T>
constexpr bool isInvalidID(const T& id) noexcept
{
    return IdTraits<T>::isInvalid(id);
}

template <typename T>
inline std::string idToString(const T& id)
{
    return IdTraits<T>::toString(id);
}

} // namespace Base

} // namespace Voxol::Render
#endif