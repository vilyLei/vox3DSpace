#ifndef VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H
#define VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H

#include "IntentComponent.h"
#include "../Tile/TileSystem.h"
#include <functional>

namespace Voxol::Intent
{
class InteractionSourceSystem
{
public:
    using SP = std::shared_ptr<InteractionSourceSystem>;
    using WP = std::weak_ptr<InteractionSourceSystem>;
    using UP = std::unique_ptr<InteractionSourceSystem>;

public:
    static InteractionSourceSystem::SP make();

public:
    InteractionSourceSystem()  = default;
    ~InteractionSourceSystem() = default;

public:
    Scene::EntityCompStorage::SP compStorage;
    Tile::TileSystem::SP          tileSys;

public:
    void initialize();
    void addSource(const Interaction::InteractionSource& srcNode);
    void singalParse(Interaction::InteractionSource& srcNode, bool selectionFlag, const std::string& actDesc);
    void updateSourceAct(const Base::ID::KeyUint64& srcId, const std::string& actDesc);
    void foreachSrcNode(const Interaction::SourceCallbackType& callback);

    void execActToDsiplay(Interaction::InteractionSource& srcNode, Interaction::MouseStatus status, std::string actDesc);
    void singalToBehavior(Interaction::InteractionSource& srcNode);
    void update();

private:
    Base::ID::keyUint64Unordered_map<Interaction::InteractionSource> srcMap;
};


} // namespace Voxol::Intent
#endif