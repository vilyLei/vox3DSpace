
#include "InteractionDataCenter.h"
#include "../Tile/TileSystem.h"
#include <functional>

namespace Voxol::Intent
{

InteractionDataCenter::SP InteractionDataCenter::make()
{
    auto sp = std::make_shared<InteractionDataCenter>();
    return sp;
}

void InteractionDataCenter::initialize() {}
void InteractionDataCenter::addSource(const Interaction::InteractionSource& srcNode)
{
    srcMap[srcNode.id] = srcNode;
}

bool InteractionDataCenter::containsSrcId(const Base::ID::KeyUint64& srcId)
{
    return actionIDMap.contains(srcId.protoId());
}

Base::ID::KeyUint64 InteractionDataCenter::findSrcId(const std::vector<Base::ID::KeyUint64>& qeIds)
{
    auto tot = qeIds.size();
    for (auto i = 0; i < tot; i++)
    {
        if (actionIDMap.contains(qeIds[i].protoId()))
            return qeIds[i];
    }
    return Base::ID::INVALID_KEY;
}

void InteractionDataCenter::foreachSrcNode(const Interaction::SourceCallbackType& callback)
{
    if (!callback)
        return;

    for (auto& item : srcMap)
    {
        auto& obj = item.second;
        callback(obj);
        obj.flags = Interaction::MouseStatus::None;
        obj.dirty = false;
    }
}
void InteractionDataCenter::update() {
}

} // namespace Voxol::Intent