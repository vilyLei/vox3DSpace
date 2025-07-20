#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"

#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{
class OglRenderer
{

public:
    OglRenderer()  = default;
    ~OglRenderer() = default;

public:
    void init();

private:
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
