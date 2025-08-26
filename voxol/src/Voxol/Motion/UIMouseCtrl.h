#ifndef VOXOL_MOTION_MOUSE_CTRL_H
#define VOXOL_MOTION_MOUSE_CTRL_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{
struct UIMouseParam
{
    float x = 0;
    float y = 0;
    int type = 0;
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