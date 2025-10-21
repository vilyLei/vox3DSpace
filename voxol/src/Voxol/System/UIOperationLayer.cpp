#include "UIOperationLayer.h"
namespace Voxol::System
{
void UIOperationLayer::initialize()
{
    mouseCtrl.tileSys   = tileSys;
    mouseCtrl.targetSys = etRenderSys;
}

void UIOperationLayer::updateKeyboardParams(int key, int scancode, int action, int mods)
{
    shortcutMana.handleKeyEvent(key, scancode, action, mods);
}
void UIOperationLayer::updateMouseParams(const Render::Draw::DrawContext& ctx, const System::Mouse::MouseInputParam& param)
{

    mouseCtrl.upateLeftMouseParam(ctx, param);
}
} // namespace Voxol::System