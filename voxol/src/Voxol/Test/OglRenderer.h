#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "OglResUtils.h"
#include "OglTestScene.h"

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
    virtual ~OglRenderer() = default;

public:
    void init();

public:
    GLuint         ctxWidth = 1200;
    GLuint         ctxHeight = 900;
    std::vector<uint8_t> cmdBuf{};
    std::function<void(GLuint, GLuint)> onDraw;

private:

    int initCtx();
    void initRenderRes();
    void render();
    void draw();


private:
    OglTestScene     mScene{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
