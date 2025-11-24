#ifndef VOXOL_INTENT_COMPONENT_H
#define VOXOL_INTENT_COMPONENT_H

#include "intentPreDef.h"
#include "EnumWrapper.h"
#include "../Base/IDDef.h"
//#include "../Scene/EntityCompStorage.h"
#include "../Scene/EntityComponent.h"
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
    Base::ID::KeyUint64                    id;
    Intent::Flag::EnumWrapper<MouseStatus> flags;
    bool                                   active = false;
    bool                                   hit    = false;
};

struct InteractionTargetDesc
{
    uint32_t            id = 0;
    Base::ID::KeyUint64 entityKey;
    std::string         type;
    std::string         command;
    uint32_t            color   = 0xff000000;
    bool                visible = true;
    void                apply() {

    }
};


//struct InteractionNodeName
//{
//    uint32_t    id = Base::ID::INVALID_ID;
//    std::string name;
//};
using InteractionNodeName = Scene::Component::UnitIdName;

struct InteractionTargetSet
{
    uint8_t                            flag = 0;
    std::string                        type = "default";
    std::vector<InteractionTargetDesc> targets;

    static InteractionTargetSet makeFromeMouseStatus(MouseStatus status)
    {
        InteractionTargetSet tar;
        tar.flag = static_cast<uint8_t>(status);
        return tar;
    }

};

struct InteractionSource
{
    Base::ID::KeyUint64 id;
    std::string         type = "default";

    Intent::Flag::EnumWrapper<Interaction::MouseStatus> flags = Interaction::MouseStatus::None;
    //for example, some mouse button actions: out, over, moving, down, up
    std::string actDesc = "out";
    bool        active  = false; // mouse hit and mouse down
    bool        hit     = false;
    bool        dirty   = false;

    std::unordered_map<uint8_t, InteractionTargetSet> tars;
    void addTargetSet(const InteractionTargetSet& targetSet)
    {
        tars[targetSet.flag] = targetSet;
    }
};

using SourceCallbackType = std::function<void(InteractionSource& srcNode)>;
} // namespace Interaction

namespace Component
{

}
}
#endif