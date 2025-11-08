#ifndef VOXOL_RENDER_OGL_FBO_H
#define VOXOL_RENDER_OGL_FBO_H

#include "DrawParam.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <array>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Render
{
namespace Draw
{
class OglFbo
{
public:
    OglFbo() = default;
    virtual ~OglFbo();

public:
    void   init(GLuint fbo);
    void   bindTextureAt(GLuint fboTex, int index, int width, int height);
    GLuint getTextureAt(int index) const;
    void   bindFBO();
    void   buildTexData(bool mipmap);
    void   renderBegin(const Draw::ClearParams& clearParam);
    void   unbindFBO();
    void   unbindFBO(const Draw::ClearParams& clearParam, bool mipmap = true);
    void   dispose();

private:
    int    mBindTexTimes = 0;
    GLuint mFbo          = GL_ZERO;
    GLuint mColorTex     = GL_ZERO;
};
} // namespace Draw
} // namespace Voxol::Render
#endif