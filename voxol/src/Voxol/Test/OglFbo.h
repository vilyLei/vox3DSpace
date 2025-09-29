#ifndef VOXOL_OGL_FBO_H
#define VOXOL_OGL_FBO_H

#    include "../Base/BaseDefine.h"

#    include <iostream>
#    include <cmath>
#    include <vector>
#    include <array>
#    include <functional>

#    include <GL/glew.h>
#    include <glfw3.h>

namespace Voxol::Test
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
    void   renderBegin(const std::array<int, 4>& viewportParams, const std::array<float, 4>& clearColor);
    void   unbindFBO(const std::array<int, 4>& viewportParams, const std::array<float, 4>& clearColor);
    void   dispose();

private:
    int mBindTexTimes    = 0;
    GLuint mFbo    = GL_ZERO;
    GLuint mColorTex = GL_ZERO;
};


} // namespace Voxol::Test
#endif