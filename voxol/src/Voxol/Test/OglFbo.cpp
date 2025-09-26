#include "OglFbo.h"
namespace Voxol::Test
{

OglFbo::~OglFbo()
{
    dispose();
}
void OglFbo::init(GLuint fbo)
{
    if (mFbo < 1)
    {
        glGenFramebuffers(1, &mFbo);
    }
}

GLuint OglFbo::getTextureAt(int index) const
{
    return mColorTex;
}
void OglFbo::bindTextureAt(GLuint fboTex, int width, int height, int index)
{
    mColorTex = fboTex;
    if (fboTex <= GL_ZERO)
    {
        glGenTextures(1, &mColorTex);
    }

    glBindTexture(GL_TEXTURE_2D, mColorTex);
    if (fboTex == GL_ZERO)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);

    mBindTexTimes++;
}
void OglFbo::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
}
void OglFbo::unbindFBO()
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