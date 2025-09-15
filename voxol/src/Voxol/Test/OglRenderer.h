#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "OglResUtils.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{

class OglRenderer
{
public:
    OglRenderer()  = default;
    ~OglRenderer();

public:
    void init();

public:
    GLuint         ctxWidth = 800;
    GLuint         ctxHeight = 600;
    std::vector<uint8_t> cmdBuf{};
    std::function<void(GLuint, GLuint)> onDraw;

private:

    int initCtx();
    void initRenderRes();
    void render();
    void draw();

private:

    Gpu::DrawingUnit baseDrawUnit{};
    Gpu::DrawingUnit texDrawUnit{};
    Gpu::DrawingUnit redFormatexDrawUnit{};
    Gpu::DrawingUnit glyphDrawUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
