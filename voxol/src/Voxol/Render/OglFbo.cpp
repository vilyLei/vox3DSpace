#include "OglFbo.h"

namespace Voxol::Render
{
namespace Draw
{

OglFbo::SP OglFbo::make()
{
    auto sp = std::make_shared<OglFbo>();
    return sp;
}

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
    else
    {
        mFbo = fbo;
    }
}

GLuint OglFbo::getTextureAt(int index) const
{
    return mColorTex;
}

//void bindFBOWithHandle(GLuint fboHandle)
//{
//    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
//}
void OglFbo::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
}
void OglFbo::renderBegin(const Draw::ClearParams& clearParam)
{
    clearParam.apply();
}
void OglFbo::bindTextureAt(GLuint fboTex, int index, int width, int height)
{
    mColorTex = fboTex;
    if (mColorTex <= GL_ZERO)
    {
        glGenTextures(1, &mColorTex);
    }

    if (mColorTex > GL_ZERO)
    {
        glBindTexture(GL_TEXTURE_2D, mColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);
    }

    mBindTexTimes++;
}
void OglFbo::buildTexData(bool mipmap)
{
    if (mColorTex == GL_ZERO)
        return;

    glBindTexture(GL_TEXTURE_2D, mColorTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mipmap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}
void OglFbo::unbindFBO()
{
    glBindTexture(GL_TEXTURE_2D, GL_ZERO);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_ZERO);
}
void OglFbo::unbindFBO(const Draw::ClearParams& clearParam, bool mipmap)
{
    buildTexData(mipmap);
    unbindFBO();

    clearParam.apply();
}
void OglFbo::unbindFBOWithViewport(const Draw::ClearParams& clearParam, bool mipmap)
{
    buildTexData(mipmap);
    unbindFBO();

    clearParam.applyViewport();
}

void OglFbo::dispose()
{
    if (mFbo != GL_ZERO)
    {
        glDeleteFramebuffers(1, &mFbo);
        mFbo = GL_ZERO;
    }
}
} // namespace Draw
} // namespace Voxol::Render