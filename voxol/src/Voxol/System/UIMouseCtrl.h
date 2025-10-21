#ifndef VOXOL_SYSTEM_MOUSE_CTRL_H
#define VOXOL_SYSTEM_MOUSE_CTRL_H

#include "../Base/BaseDefine.h"
#include "../Motion/RenderCmdComp.h"
#include "../System/MouseEventSystem.h"

namespace Voxol::System
{
class UIMouseCtrl
{
private:
public:
    UIMouseCtrl()  = default;
    ~UIMouseCtrl() = default;

public:
    bool setMouseParams(Motion::ViewComponent& view, const Mouse::MouseInputParam& param);
};

} // namespace Voxol::Motion
#endif