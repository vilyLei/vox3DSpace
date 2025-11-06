#include "CompBaseDef.h"
#include <string_view>
#include <sstream>
#include <iomanip>

namespace Voxol::Render
{

namespace ID
{

uint32_t EntityIdManager::allocate()
{
    if (!freeList.empty())
    {
        auto id = freeList.back();
        freeList.pop_back();
        return id;
    }
    if (ID::isInvalidID(nextId))
    {
        printf("EntityIdManager::allocate(), EntityId overflow: exceeded 28-bit range.\n");
        return INVALID_ID;
    }
    return nextId++;
}

void EntityIdManager::release(uint32_t id)
{
    freeList.push_back(id);
}

std::string KeyUint64::idToString(bool hex) const
{
    std::ostringstream oss;
    if (hex)
    {
        oss << std::hex << std::setfill('0')
            << "KeyUint64(iid=0x" << std::setw(6) << iid()
            << ", proto=0x" << std::setw(6) << protoId()
            << ")";
    }
    else
    {
        oss << "KeyUint64(iid=" << iid()
            << ", proto=" << protoId()
            << ")";
    }
    return oss.str();
}
std::string KeyUint64::toString(bool hex) const
{
    std::ostringstream oss;
    if (hex)
    {
        oss << std::hex << std::setfill('0')
            << "KeyUint64(flags=0x" << std::setw(4) << std::to_string(flags())
            << " iid=0x" << std::setw(6) << iid()
            << " proto=0x" << std::setw(6) << protoId()
            << " [value=0x" << std::setw(16) << value << "])";
    }
    else
    {
        oss << "KeyUint64(flags=" << std::to_string(flags())
            << ",iid=" << iid()
            << ",proto=" << protoId()
            << ",[value=" << value << "])";
    }
    return oss.str();
}


uint32_t KeyUint64Manager::IIDPool::allocate()
{
    if (!freeList.empty())
    {
        uint32_t id = freeList.back();
        freeList.pop_back();
        return id;
    }
    if (ID::isInvalidID(nextIID))
        return INVALID_ID;
    return nextIID++;
}

void KeyUint64Manager::IIDPool::release(uint32_t iid)
{
    if (iid < INVALID_ID && iid != 0)
        freeList.push_back(iid);
}

[[nodiscard]] bool KeyUint64Manager::IIDPool::empty() const noexcept
{
    return freeList.empty() && nextIID <= 1;
}

[[nodiscard]] size_t KeyUint64Manager::IIDPool::freeCount() const noexcept { return freeList.size(); }


[[nodiscard]] KeyUint64 KeyUint64Manager::allocate(uint32_t protoId, uint8_t flags)
{
    assert(protoId < INVALID_ID && "protoId must be valid");
    auto&&    pool = protoPools[protoId];
    auto  iid  = pool.allocate();
    return KeyUint64::make(protoId, iid, flags);
}

[[nodiscard]] KeyUint64 KeyUint64Manager::allocate(EntityId protoId, uint8_t flags)
{
    return allocate(protoId.value, flags);
}

void KeyUint64Manager::release(const KeyUint64& key)
{
    if (key.isIDInvalid())
        return;

    auto protoId = key.protoId();
    auto iid     = key.iid();

    auto&& it = protoPools.find(protoId);
    if (it != protoPools.end())
    {
        it->second.release(iid);
    }
}

[[nodiscard]] size_t KeyUint64Manager::protoCount() const noexcept
{
    return protoPools.size();
}

[[nodiscard]] size_t KeyUint64Manager::freeCount(uint32_t protoId) const noexcept
{
    auto&& it = protoPools.find(protoId);
    if (it == protoPools.end())
        return 0;
    return it->second.freeCount();
}

[[nodiscard]] bool KeyUint64Manager::hasProto(uint32_t protoId) const noexcept
{
    return protoPools.find(protoId) != protoPools.end();
}

void KeyUint64Manager::clear()
{
    protoPools.clear();
}

} // namespace ID

} // namespace Voxol::Render