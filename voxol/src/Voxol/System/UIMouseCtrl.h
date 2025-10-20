#ifndef VOXOL_SYSTEM_MOUSE_CTRL_H
#define VOXOL_SYSTEM_MOUSE_CTRL_H

#include "../Base/BaseDefine.h"
#include "../Motion/RenderCmdComp.h"

namespace Voxol::System
{
enum class UIMouseActionPhase : unsigned char
{
    Begin   = 0,
    Dragging     = 1,
    End   = 3
};
enum class UIMouseType : unsigned char
{
    MOUSE_DOWN = 11,
    MOUSE_UP = 12,
    MOUSE_MOVE = 3,
    MOUSE_SCROLL = 4,
    MOUSE_CLICK = 5
};

struct UIMouseParam
{
    float x = 0;
    float y = 0;
    UIMouseType type  = UIMouseType::MOUSE_DOWN;
    float value = 0;
};

class UIMouseCtrl
{
private:
public:
    UIMouseCtrl()  = default;
    ~UIMouseCtrl() = default;

public:
    bool setMouseParams(Motion::ViewComponent& view, const UIMouseParam& param);
};

} // namespace Voxol::Motion
#endif