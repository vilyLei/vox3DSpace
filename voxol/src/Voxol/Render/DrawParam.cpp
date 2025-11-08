
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
    printf("ClearParams::applyViewport(), viewport(x=%d,y=%d,w=%d,h=%d)\n", viewport.x, viewport.y, viewport.width, viewport.height);
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void ClearParams::applyClearColor() const
{
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearMask);
}
void ClearParams::apply(bool info) const
{
    if (info)
    {
        printf("ClearParams::apply(), viewport(x=%d,y=%d,w=%d,h=%d)\n", viewport.x, viewport.y, viewport.width, viewport.height);
    }
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    glClear(clearMask);
}

} // namespace Draw
} // namespace Voxol::Render