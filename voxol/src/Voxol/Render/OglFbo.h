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
    using SP = std::shared_ptr<OglFbo>;
    using WP = std::weak_ptr<OglFbo>;
    using UP = std::unique_ptr<OglFbo>;

public:
    static OglFbo::SP make();

public:
    OglFbo() = default;
    virtual ~OglFbo();

public:
    void   init(GLuint fbo);
    void   bindTextureAt(GLuint fboTex, int index, int width, int height);
    GLuint getTextureAt(int index) const;
    //void   bindFBOWithHandle(GLuint fboHandle);
    void   bindFBO();
    void   buildTexData(bool mipmap);
    void   renderBegin(const Draw::ClearParams& clearParam);
    void   unbindFBO();
    void   unbindFBO(const Draw::ClearParams& clearParam, bool mipmap = true);
    void   unbindFBO(bool mipmap);
    void   dispose();

private:
    int    mBindTexTimes = 0;
    GLuint mFbo          = GL_ZERO;
    GLuint mColorTex     = GL_ZERO;
};
} // namespace Draw
} // namespace Voxol::Render
#endif