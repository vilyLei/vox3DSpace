
#include <functional>
#include "DrawParam.h"
#include "../Math/VxRect.h"
#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Render
{
namespace Draw
{

void ClearParams::applyViewport() const
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void ClearParams::applyClearColor() const
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearMask);
}
void ClearParams::apply() const
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearMask);
}

} // namespace Draw
} // namespace Voxol::Render