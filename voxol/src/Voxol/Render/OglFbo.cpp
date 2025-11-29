#include "OglFbo.h"

namespace Voxol::Render
{
namespace Draw
{
int OglFbo::sUid = 0;

OglFbo::SP OglFbo::make()
{
    auto sp = std::make_shared<OglFbo>();
    return sp;
}

OglFbo::~OglFbo()
{
    dispose();
}

int OglFbo::uid() const
{
    return mUid;
}
GLuint OglFbo::fboHandle() const
{
    return mFbo;
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

bool OglFbo::fboBinding() const
{
    return fboBindingFlag;
}
void OglFbo::bindFBO()
{
    //printf("OglFbo::bindFBO(), fbo(%d), handle:(mFbo=%d), fboBindingFlag: %d\n", mUid, mFbo, fboBindingFlag);
    if (fboBindingFlag)
        return;

    //printf("OglFbo::bindFBO(), fbo(%d), handle:(mFbo=%d), mColorTex: %d\n", mUid, mFbo, mColorTex);
    fboBindingFlag = true;
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
}

void OglFbo::unbindFBO()
{
    if (!fboBindingFlag)
        return;

    //printf("OglFbo::unbindFBO(), fbo(%d), handle:(mFbo=%d), mColorTex: %d\n", mUid, mFbo, mColorTex);
    fboBindingFlag = false;
    glBindTexture(GL_TEXTURE_2D, GL_ZERO);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_ZERO);
}
void OglFbo::renderBegin(const Draw::ClearParams& clearParam)
{
    clearParam.apply();
}

void OglFbo::bindTextureAt(GLuint fboTex, int index, int width, int height)
{

    mBindTexTimes++;
    mColorTex = fboTex;
    if (mColorTex == GL_ZERO || mFbo == GL_ZERO)
    {
        glGenTextures(1, &mColorTex);
    }

    if (mColorTex == GL_ZERO)
        return;

    //printf("OglFbo::bindTextureAt(), fbo(%d), handle:(mFbo=%d), mColorTex: %d\n", mUid, mFbo, mColorTex);

    textureDirty = true;

    glBindTexture(GL_TEXTURE_2D, mColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTex, 0);
}
void OglFbo::buildTexData(bool mipmap)
{
    if (!textureDirty || mColorTex == GL_ZERO)
        return;

    //printf("OglFbo::buildTexData(), fbo(%d), handle:(mFbo=%d), mColorTex: %d\n", mUid, mFbo, mColorTex);
    textureDirty = false;
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
void OglFbo::unbindFBO(const Draw::ClearParams& clearParam, bool mipmap)
{
    buildTexData(mipmap);
    unbindFBO();

    clearParam.apply();
}
void OglFbo::unbindFBO(bool mipmap)
{
    buildTexData(mipmap);
    unbindFBO();
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