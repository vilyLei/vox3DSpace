#ifndef VOXOL_OGL_RENDERER_H
#define VOXOL_OGL_RENDERER_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "OglResUtils.h"
#include "OglTestScene.h"
#include "../Motion/RenderCmdComp.h"
#include "../Motion/UIMouseCtrl.h"

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
    OglRenderer()          = default;
    virtual ~OglRenderer() = default;

public:
    void init();

public:
    GLuint                              ctxWidth  = 8;
    GLuint                              ctxHeight = 8;
    std::vector<uint8_t>                cmdBuf{};
    std::function<void(GLuint, GLuint)> onDraw;

private:
    int  initCtx();
    void initRenderRes();
    void render();
    void draw();


    Voxol::Motion::Point2DDesc mousePos{};
    Voxol::Motion::CanvasDesc  canvas{};
    Voxol::Motion::UIMouseCtrl mouseCtrl{};

private:
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouseEnter_callback(GLFWwindow* window, int flag);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void mousePos_callback(GLFWwindow* window, double posX, double posY);
    static void mouseButton_callback(GLFWwindow* window, int sign, int flag, int type);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static GLuint ctxCurrWidth;
    static GLuint ctxCurrHeight;

    void setMouseXY(float x, float y);

    void setMouseParams(const Voxol::Motion::UIMouseParam& param);

    OglTestScene mScene{};
    bool         dirty = true;
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
