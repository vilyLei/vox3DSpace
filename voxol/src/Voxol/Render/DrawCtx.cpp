#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{


void FBOContext::bindFBO(bool onlyChangeViewport) const
{
    printf("FBOContext::bindFBO() fbo(%d), onlyChangeViewport: %d\n", fbo->uid(), onlyChangeViewport);
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
    printf("FBOContext::unbindFBO() fbo(%d), texUnits.size(): %lld\n", fbo->uid(), texUnits.size());
    if (texUnits.empty())
    {
        fbo->unbindFBO(false);
    }
    else
    {
        auto&& tex = texUnits[0];
        fbo->unbindFBO(tex.mipmap);
    }
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

void FBOContext::buildTexData() const
{
    if (!texUnits.empty())
    {
        auto&& tex = texUnits[0];
        fbo->buildTexData(tex.mipmap);
    }
}

GLuint FBOContext::getTextureAt(int index) const
{
    if (texUnits.empty() || index < 0 || index >= texUnits.size())
        return 0;

    buildTexData();
    auto&& tex = texUnits[index];
    tex.texture = fbo->getTextureAt(index);
    return tex.texture;
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
    printf("DrawContext::renderBeginWithFBOCtx() ...\n");

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
        printf("DrawContext::renderEndWithFBOCtx() A ...\n");
        preFCtx.bindFBO(true);
        return;
    }

    fctx.unbindFBO();
    clearParam.applyViewport();
    printf("DrawContext::renderEndWithFBOCtx() B ...\n");
}
bool DrawContext::hasFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return !stack.empty();
}
bool DrawContext::hasNotFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.empty();
}
void DrawContext::pushFBOCtx(const FBOContext& fboCtx) const
{
    printf("DrawContext::pushFBOCtx() ...\n");
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
        stack.emplace_back(ctx);
    }
}
void DrawContext::popFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    if (stack.empty())
        return;

    auto&& ctx   = stack.back();
    auto&& fbo   = ctx.fbo;
    if (fbo->fboBinding())
    {
        renderEndWithFBOCtx();
    }
    if (fbo) {
        fboCtxStack.fboStack.emplace_back(fbo);
    }
    stack.pop_back();
    printf("DrawContext::popFBOCtx() ...\n");
}
const FBOContext& DrawContext::topFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.back();
}

GLuint DrawContext::getFBOTextureAt(int index) const
{
    if (index < 0 || fboCtxStack.ctxStack.empty())
        return 0;

    auto& ctx = fboCtxStack.ctxStack.back();
    auto  tex = ctx.getTextureAt(index);
    printf("DrawContext::getFBOTextureAt() tex: %d\n", tex);
    return tex;
}
} // namespace Draw
} // namespace Voxol::Render
