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
    OglFbo::SP                  fbo;
    ClearParams                 clearParam{};
    std::vector<OglTextureUnit> textures;

    void bindFBO() const;
    void unbindFBO() const;
    void applyViewport() const;
    void applyClearColor() const;
    void applyClearViewport() const;
};
struct DrawContext
{
    std::vector<FBOContext> fboCtxStack;

    ClearParams clearParam{};
    DrawParams  drawParam{};

    DrawCallType  drawCall;
    DrawQueryType drawQueryCall;

    void applyViewport() const;
    void applyClearColor() const;
    void applyClearViewport() const;

    void              bindFBOCtx();
    void              renderBeginWithFBOCtx();
    void              renderEndWithFBOCtx();
    bool              hasFBOCtx();
    bool              hasNotFBOCtx();
    void              pushFBOCtx(const FBOContext& fboCtx);
    void              popFBOCtx();
    const FBOContext& topFBOCtx();

    float zoom  = 1;
    bool  dirty = true;
};
} // namespace Draw
} // namespace Voxol::Render
#endif