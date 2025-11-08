#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{


void FBOContext::bindFBO(bool onlyChangeViewport) const
{
    fbo->bindFBO();
    for (auto& tex : texUnits)
    {
        fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
    }
    if (onlyChangeViewport)
    {
        clearParam.applyViewport();
    }
    else
    {
        clearParam.apply();
    }
}
void FBOContext::unbindFBO() const
{
    auto& tex = texUnits[0];
    fbo->unbindFBO(tex.mipmap);
}
void FBOContext::applyViewport() const
{
    clearParam.applyViewport();
}

void FBOContext::applyClearColor() const
{
    clearParam.applyClearColor();
}

void FBOContext::applyClearViewport() const
{
    clearParam.apply();
}

GLuint FBOContext::getTextureAt(int index) const
{
    return texUnits[index].texture;
}


void DrawContext::applyViewport() const
{
    clearParam.applyViewport();
}

void DrawContext::applyClearColor() const
{
    clearParam.applyClearColor();
}

void DrawContext::applyClearViewport() const
{
    clearParam.apply();
}

void DrawContext::bindFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    auto  fctx  = stack.back();
    fctx.fbo->bindFBO();
}
void DrawContext::renderBeginWithFBOCtx() const
{
    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();
    fctx.bindFBO(false);
}

void DrawContext::renderEndWithFBOCtx() const
{
    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();

    if (stack.size() > 1)
    {
        auto&& preFCtx = stack[stack.size() - 2];

        fctx.unbindFBO();
        preFCtx.bindFBO(true);

        return;
    }

    fctx.unbindFBO();
    clearParam.applyViewport();
}
bool DrawContext::hasFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.empty();
}
bool DrawContext::hasNotFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return !stack.empty();
}
void DrawContext::pushFBOCtx(const FBOContext& fboCtx) const
{
    auto& stack = fboCtxStack.ctxStack;
    if (fboCtx.fbo)
    {
        stack.emplace_back(fboCtx);
    }
    else {

        auto&      fboStack = fboCtxStack.fboStack;
        OglFbo::SP fbo;
        if (fboStack.empty())
        {
            fbo = OglFbo::make();
            fbo->init(GL_ZERO);
        }
        else {
            fbo = fboStack.back();
            fboStack.pop_back();
        }
        FBOContext ctx = fboCtx;
        ctx.fbo        = fbo;
    }
}
void DrawContext::popFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    stack.pop_back();
}
const FBOContext& DrawContext::topFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.back();
}
} // namespace Draw
} // namespace Voxol::Render
