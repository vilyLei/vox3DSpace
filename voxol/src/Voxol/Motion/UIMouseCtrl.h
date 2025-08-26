#ifndef VOXOL_MOTION_MOUSE_CTRL_H
#define VOXOL_MOTION_MOUSE_CTRL_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{
class UIMouseCtrl
{
private:
public:
    UIMouseCtrl()  = default;
    ~UIMouseCtrl() = default;

public:
    void setMouseParams(ViewComponent& view, float x, float y, int type, float value);
};

} // namespace Voxol::Motion
#endif