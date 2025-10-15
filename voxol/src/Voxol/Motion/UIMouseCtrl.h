#ifndef VOXOL_MOTION_MOUSE_CTRL_H
#define VOXOL_MOTION_MOUSE_CTRL_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{
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
    bool setMouseParams(ViewComponent& view, const UIMouseParam& param);
};

} // namespace Voxol::Motion
#endif