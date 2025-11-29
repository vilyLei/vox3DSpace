#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{


void FBOCtxNode::bindFBO(bool onlyChangeViewport) const
{
    //printf("FBOCtxNode::bindFBO() fbo(%d), onlyChangeViewport: %d\n", fbo->uid(), onlyChangeViewport);

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
void FBOCtxNode::unbindFBO() const
{
    //printf("FBOCtxNode::unbindFBO() fbo(%d), texUnits.size(): %lld\n", fbo->uid(), texUnits.size());
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
void FBOCtxNode::applyViewport() const
{
    clearParam.applyViewport();
}

void FBOCtxNode::applyClearColor() const
{
    clearParam.applyClearColor();
}

void FBOCtxNode::applyClearViewport() const
{
    clearParam.apply();
}

void FBOCtxNode::buildTexData() const
{
    if (!texUnits.empty())
    {
        auto&& tex = texUnits[0];
        fbo->buildTexData(tex.mipmap);
    }
}

GLuint FBOCtxNode::getRTTextureAt(int index) const
{
    if (texUnits.empty() || index < 0 || index >= texUnits.size())
        return 0;

    buildTexData();
    auto&& tex = texUnits[index];
    tex.texture = fbo->getTextureAt(index);
    return tex.texture;
}


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< fbo render graph begin

void FBORenderGraph::applyViewport() const
{
    backgroundClearParam.applyViewport();
}

void FBORenderGraph::applyClearColor() const
{
    backgroundClearParam.applyClearColor();
}

void FBORenderGraph::applyClearViewport() const
{
    backgroundClearParam.apply();
}

void FBORenderGraph::bindNode() const
{
    auto& stack = fboCtxStack.ctxStack;
    auto  fctx  = stack.back();
    fctx.fbo->bindFBO();
}
void FBORenderGraph::renderBegin() const
{
    //printf("FBORenderGraph::renderBeginWithFBOCtx() ...\n");

    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();
    fctx.bindFBO(false);
}

void FBORenderGraph::renderEnd() const
{
    auto&  stack = fboCtxStack.ctxStack;
    auto&& fctx  = stack.back();
    fctx.unbindFBO();
    //printf("FBORenderGraph::renderEndWithFBOCtx() ...\n");
}
bool FBORenderGraph::hasNode() const
{
    auto& stack = fboCtxStack.ctxStack;
    return !stack.empty();
}
bool FBORenderGraph::hasNotNode() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.empty();
}
void FBORenderGraph::pushNode(const FBOCtxNode& fboCtx, bool autoRenderBegin) const
{
    //printf("FBORenderGraph::pushFBOCtx() ...\n");
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

        auto&      fboStack = fboCtxStack.freeFboStack;
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
        FBOCtxNode ctx = fboCtx;
        ctx.fbo        = fbo;
        stack.emplace_back(ctx);
    }
    if (autoRenderBegin)
    {
        renderBegin();
    }
}
void FBORenderGraph::popNode() const
{
    auto& stack = fboCtxStack.ctxStack;
    if (stack.empty())
        return;

    auto&& ctx = stack.back();
    auto&& fbo = ctx.fbo;
    if (fbo->fboBinding())
    {
        renderEnd();
    }
    if (fbo)
    {
        fboCtxStack.freeFboStack.emplace_back(fbo);
    }
    stack.pop_back();

    //printf("FBORenderGraph::popFBOCtx() stack.empty(): %d\n", stack.empty());

    if (stack.empty())
    {
        //printf("FBORenderGraph::popFBOCtx() switch to background buffer ...\n");
        backgroundClearParam.applyViewport();
        return;
    }

    auto&& nextCtx = stack.back();
    //printf("FBORenderGraph::popFBOCtx() switch to orther rtt ...\n");
    nextCtx.bindFBO(true);
}
const FBOCtxNode& FBORenderGraph::topNode() const
{
    auto& stack = fboCtxStack.ctxStack;
    return stack.back();
}

GLuint FBORenderGraph::getRTTextureAt(int index) const
{
    if (index < 0 || fboCtxStack.ctxStack.empty())
        return 0;

    auto& ctx = fboCtxStack.ctxStack.back();
    auto  tex = ctx.getRTTextureAt(index);
    //printf("FBORenderGraph::getRTTextureAt() tex: %d\n", tex);
    return tex;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> fbo render graph end

GLuint DrawContext::getRTTextureAt(int index) const
{
    return fboGraph.getRTTextureAt(index);
}

} // namespace Draw
} // namespace Voxol::Render
