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
    //fctx.fbo->bindFBO();
    //auto  texIndex = 0;
    //auto& tex      = fctx.textures[texIndex];
    if (stack.size() > 1)
    {
        auto&& preFCtx = stack[stack.size() - 2];

        //fctx.fbo->unbindFBOWithViewport(preFCtx.clearParam, tex.mipmap);
        //fctx.fbo->unbindFBO(tex.mipmap);
        //preFCtx.fbo->bindFBO();
        //preFCtx.applyViewport();
        fctx.unbindFBO();
        preFCtx.bindFBO(true);

        //for (auto& tex : preFCtx.textures)
        //{
        //    preFCtx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
        //}
        return;
    }
    //fctx.fbo->unbindFBOWithViewport(clearParam, tex.mipmap);
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
    stack.emplace_back(fboCtx);
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
