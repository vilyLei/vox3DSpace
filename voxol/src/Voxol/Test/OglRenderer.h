#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{
namespace OglTest
{

}
class OglRenderer
{

public:
    OglRenderer()  = default;
    ~OglRenderer() = default;

public:
    void init();

private:

    int initCtx();
    void initRender();
    void render();

    GLuint program;
    GLuint vao;
    GLint  matrixLoc;
    GLint  colorLoc;
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
