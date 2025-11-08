#ifndef VOXOL_RENDER_DRAW_CTX_H
#define VOXOL_RENDER_DRAW_CTX_H

#include <functional>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "DrawParam.h"
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>
#include "OglFbo.h"

namespace Voxol::Render
{
namespace Draw
{

struct FBOContext
{
    Math::Mat33                 viewMat;
    mutable OglFbo::SP          fbo;
    ClearParams                 clearParam{};
    std::vector<OglTextureUnit> texUnits;

    void bindFBO(bool onlyChangeViewport) const;
    void unbindFBO() const;
    void applyViewport() const;
    void applyClearColor() const;
    void applyClearViewport() const;
    void buildTexData() const;

    GLuint getTextureAt(int index) const;
};
struct FBOCtxStack
{
    uint32_t                depth = 0;
    std::vector<OglFbo::SP> fboStack;
    std::vector<FBOContext> ctxStack;
};
struct DrawContext
{
    mutable FBOCtxStack fboCtxStack;

    ClearParams clearParam{};
    DrawParams  drawParam{};

    DrawCallType  drawCall;
    DrawQueryType drawQueryCall;

    void applyViewport() const;
    void applyClearColor() const;
    void applyClearViewport() const;

    void              bindFBOCtx() const;
    void              renderBeginWithFBOCtx() const;
    void              renderEndWithFBOCtx() const;
    bool              hasFBOCtx() const;
    bool              hasNotFBOCtx() const;
    void              pushFBOCtx(const FBOContext& fboCtx) const;
    void              popFBOCtx() const;
    const FBOContext& topFBOCtx() const;
    GLuint            getFBOTextureAt(int index) const;

    float zoom  = 1;
    bool  dirty = true;
};
} // namespace Draw
} // namespace Voxol::Render
#endif