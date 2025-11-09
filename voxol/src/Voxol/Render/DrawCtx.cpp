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


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< fbo render graph begin

void FBORenderGraph::bindFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    auto  fctx  = stack.back();
    fctx.fbo->bindFBO();
}
void FBORenderGraph::renderBeginWithFBOCtx() const
{
    printf("FBORenderGraph::renderBeginWithFBOCtx() ...\n");

    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();
    fctx.bindFBO(false);
}

void FBORenderGraph::renderEndWithFBOCtx() const
{
    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();
    fctx.unbindFBO();
    printf("FBORenderGraph::renderEndWithFBOCtx() ...\n");
}
bool FBORenderGraph::hasFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return !stack.empty();
}
bool FBORenderGraph::hasNotFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.empty();
}
void FBORenderGraph::pushFBOCtx(const FBOContext& fboCtx) const
{
    printf("FBORenderGraph::pushFBOCtx() ...\n");
    auto& stack = fboCtxStack.ctxStack;
    if (!stack.empty())
    {
        auto&& preCtx = stack.back();
        preCtx.fbo->unbindFBO();
    }

    if (fboCtx.fbo)
    {
        stack.emplace_back(fboCtx);
    }
    else
    {

        auto&      fboStack = fboCtxStack.fboStack;
        OglFbo::SP fbo;
        if (fboStack.empty())
        {
            fbo = OglFbo::make();
            fbo->init(GL_ZERO);
        }
        else
        {
            fbo = fboStack.back();
            fboStack.pop_back();
        }
        FBOContext ctx = fboCtx;
        ctx.fbo        = fbo;
        stack.emplace_back(ctx);
    }
}
void FBORenderGraph::popFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    if (stack.empty())
        return;

    auto&& ctx = stack.back();
    auto&& fbo = ctx.fbo;
    if (fbo->fboBinding())
    {
        renderEndWithFBOCtx();
    }
    if (fbo)
    {
        fboCtxStack.fboStack.emplace_back(fbo);
    }
    stack.pop_back();

    printf("FBORenderGraph::popFBOCtx() stack.empty(): %d\n", stack.empty());

    if (stack.empty())
    {
        printf("FBORenderGraph::popFBOCtx() switch to background buffer ...\n");
        backgroundClearParam.applyViewport();
        return;
    }

    auto&& nextCtx = stack.back();
    printf("FBORenderGraph::popFBOCtx() switch to orther rtt ...\n");
    nextCtx.bindFBO(true);
}
const FBOContext& FBORenderGraph::topFBOCtx() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.back();
}

GLuint FBORenderGraph::getFBOTextureAt(int index) const
{
    if (index < 0 || fboCtxStack.ctxStack.empty())
        return 0;

    auto& ctx = fboCtxStack.ctxStack.back();
    auto  tex = ctx.getTextureAt(index);
    printf("FBORenderGraph::getFBOTextureAt() tex: %d\n", tex);
    return tex;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> fbo render graph end






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

/*
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
    fctx.unbindFBO();
    printf("DrawContext::renderEndWithFBOCtx() ...\n");
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
    if (!stack.empty())
    {
        auto&& preCtx = stack.back();
        preCtx.fbo->unbindFBO();
    }
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

    printf("DrawContext::popFBOCtx() stack.empty(): %d\n", stack.empty());

    if (stack.empty())
    {
        printf("DrawContext::popFBOCtx() switch to background buffer ...\n");
        clearParam.applyViewport();
        return;
    }

    auto&& nextCtx = stack.back();
    printf("DrawContext::popFBOCtx() switch to orther rtt ...\n");
    nextCtx.bindFBO(true);
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
//*/
} // namespace Draw
} // namespace Voxol::Render
