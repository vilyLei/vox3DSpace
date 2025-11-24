#ifndef VOXOL_INTENT_INTERACTION_DATA_CENTER_H
#define VOXOL_INTENT_INTERACTION_DATA_CENTER_H

#include "IntentComponent.h"
#include "../Scene/EntityCompStorage.h"
#include <functional>

namespace Voxol::Intent
{
class InteractionDataCenter
{
public:
    using SP = std::shared_ptr<InteractionDataCenter>;
    using WP = std::weak_ptr<InteractionDataCenter>;
    using UP = std::unique_ptr<InteractionDataCenter>;

public:
    static InteractionDataCenter::SP make();

public:
    InteractionDataCenter()  = default;
    ~InteractionDataCenter() = default;

public:
    Scene::EntityCompStorage::SP                                   compStorage;
    std::unordered_map<uint32_t, Interaction::InteractionNodeName> actionIDMap;

public:
    void                initialize();
    void                addSource(const Interaction::InteractionSource& srcNode);
    bool                containsSrcId(const Base::ID::KeyUint64& srcId);
    Base::ID::KeyUint64 findSrcId(const std::vector<Base::ID::KeyUint64>& qeIds);
    void                foreachSrcNode(const Interaction::SourceCallbackType& callback);
    void update();

private:
    Base::ID::keyUint64Unordered_map<Interaction::InteractionSource> srcMap;
};


} // namespace Voxol::Intent
#endif