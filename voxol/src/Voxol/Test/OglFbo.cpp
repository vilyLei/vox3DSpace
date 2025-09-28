#include "OglFbo.h"
namespace Voxol::Test
{

OglFbo::~OglFbo()
{
    dispose();
}
void OglFbo::init(GLuint fbo)
{
    if (fbo < 1)
    {
        glGenFramebuffers(1, &mFbo);
    }
    else {
        mFbo = fbo;
    }
}

GLuint OglFbo::getTextureAt(int index) const
{
    return mColorTex;
}
void OglFbo::bindTextureAt(GLuint fboTex, int index, int width, int height)
{
    mColorTex = fboTex;
    if (fboTex <= GL_ZERO)
    {
        glGenTextures(1, &mColorTex);
    }

    glBindTexture(GL_TEXTURE_2D, mColorTex);
    if (fboTex > GL_ZERO)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);
    }

    mBindTexTimes++;
}
void OglFbo::bindFBO(int width, int height, const std::array<float, 4>& clearColor)
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    glViewport(0, 0, width, height);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}
void OglFbo::unbindFBO(int width, int height, const std::array<float, 4>& clearColor)
{

    if (mColorTex != GL_ZERO)
    {
        glBindTexture(GL_TEXTURE_2D, mColorTex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_ZERO);
    glViewport(0, 0, width, height);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OglFbo::dispose()
{
    if (mFbo != GL_ZERO)
    {
        glDeleteFramebuffers(1, &mFbo);
        mFbo = GL_ZERO;
    }
}
} // namespace Voxol::Test