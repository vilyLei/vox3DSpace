#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

#include <iostream>
#include <cmath>
#include <vector>

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
    ~OglRenderer();

public:
    void init();

public:
    std::vector<uint8_t> cmdBuf{};

private:

    int initCtx();
    void initRender();
    void render();
    void draw();

private:
    GLuint program;
    GLuint vao;
    GLint  matrixLoc;
    GLint  colorLoc;
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
