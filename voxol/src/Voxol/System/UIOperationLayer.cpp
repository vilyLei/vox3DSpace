#include "UIOperationLayer.h"
namespace Voxol::System
{
void UIOperationLayer::initialize()
{
    mouseCtrl.tileSys   = tileSys;
    mouseCtrl.targetSys = etRenderSys;
}

void UIOperationLayer::updateMouseParams(const Render::Draw::DrawContext& ctx, const System::Mouse::MouseInputParam& param)
{

    mouseCtrl.upateLeftMouseParam(ctx, param);
}
} // namespace Voxol::System