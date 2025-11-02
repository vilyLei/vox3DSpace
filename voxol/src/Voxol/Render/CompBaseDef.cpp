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
    if ((nextId+1) >= INVALID_ID)
        throw std::runtime_error("EntityId overflow: exceeded 28-bit range.");
    return nextId++;
}

void EntityIdManager::release(uint32_t id)
{
    freeList.push_back(id);
}

std::string KeyUint64::toString(bool hex) const
{
    std::ostringstream oss;
    if (hex)
    {
        oss << std::hex << std::setfill('0')
            << "KeyUint64(flags=0x" << std::setw(4) << flags()
            << " iid=0x" << std::setw(6) << iid()
            << " proto=0x" << std::setw(6) << protoNodeId()
            << " [value=0x" << std::setw(16) << value << "])";
    }
    else
    {
        oss << "KeyUint64(flags=" << flags()
            << " iid=" << iid()
            << " proto=" << protoNodeId()
            << " [value=" << value << "])";
    }
    return oss.str();
}
} // namespace Base

} // namespace Voxol::Render