#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{

void ClearParams::apply() const
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearMask);
}
}
} // namespace Voxol::Render
