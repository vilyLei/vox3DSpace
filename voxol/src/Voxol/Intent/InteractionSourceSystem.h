#ifndef VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H
#define VOXOL_INTENT_INTERACTION_SOURCE_SYSTEM_H

#include "intentPreDef.h"
#include "EnumWrapper.h"
#include "../Render/CompBaseDef.h"
#include <functional>

namespace Voxol::Intent
{
namespace Interaction
{
enum class MouseStatus : uint8_t
{
    None = 0,
    Out  = 1 << 0,
    Over = 1 << 1,
    Down = 1 << 2,
    Up   = 1 << 3,
    Move = 1 << 4
};

struct InteractionSourceFlag
{
    Render::ID::KeyUint64          id;
    Intent::Flag::EnumWrapper<MouseStatus> flags;
    bool                   active = false;
    bool                   hit    = false;
};

struct InteractionTargetDesc
{
    Render::ID::KeyUint64 id;
    std::string   type;
    uint32_t      color   = 0xff000000;
    bool          visible = true;
};

struct InteractionTargetSet
{
    uint8_t                 flag = 0;
    std::string             type = "default";
    std::vector<InteractionTargetDesc> targets;
};

struct InteractionSource
{
    Render::ID::KeyUint64 id;
    std::string   type = "default";

    Intent::Flag::EnumWrapper<MouseStatus> flags;
    //for example, some mouse button actions: out, over, moving, down, up
    std::string                               actDesc    = "out";
    bool                                      active = false; // mouse hit and mouse down
    bool                                      hit    = false;
    bool                                      dirty  = false;

    std::unordered_map<uint8_t, InteractionTargetSet>    tars;
};

using SourceCallbackType = std::function<void(InteractionSource& srcNode)>;
} // namespace Interaction

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
    void initialize();
    void addSource(const Interaction::InteractionSource& srcNode);
    void singalParse(Interaction::InteractionSource& srcNode, bool selectionFlag, const std::string& actDesc);
    void updateSrcAct(const Render::ID::KeyUint64& srcId, const std::string& actDesc);
    void foreachSrcNode(const Interaction::SourceCallbackType& callback);

    void execActToDsiplay(Interaction::InteractionSource& srcNode, Interaction::MouseStatus status, std::string actDesc);
    void singalToBehavior(Interaction::InteractionSource& srcNode);
    void update();

private:
    Render::ID::keyUint64Unordered_map<Interaction::InteractionSource> srcMap;

};


} // namespace Voxol::Intent
#endif