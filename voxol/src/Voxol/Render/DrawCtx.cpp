#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{

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

void DrawContext::bindFBOCtx()
{
    auto fctx = fboCtxStack.back();
    fctx.fbo->bindFBO();
}
void DrawContext::renderBeginWithFBOCtx()
{
    auto fctx = fboCtxStack.back();
    fctx.fbo->bindFBO();

    for (auto& tex : fctx.textures)
    {
        fctx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
    }
    //auto  texIndex = 0;
    //auto& tex      = fctx.textures[texIndex];
    //fctx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
    fctx.fbo->renderBegin(fctx.clearParam);
}

void DrawContext::renderEndWithFBOCtx()
{
    auto fctx = fboCtxStack.back();
    fctx.fbo->bindFBO();
    auto  texIndex = 0;
    auto& tex      = fctx.textures[texIndex];
    if (fboCtxStack.size() > 1)
    {
        auto&& preFCtx = fboCtxStack[fboCtxStack.size() - 2];

        //fctx.fbo->unbindFBOWithViewport(preFCtx.clearParam, tex.mipmap);
        fctx.fbo->unbindFBO(tex.mipmap);
        preFCtx.fbo->bindFBO();
        preFCtx.applyViewport();

        for (auto& tex : preFCtx.textures)
        {
            preFCtx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
        }
        return;
    }
    fctx.fbo->unbindFBOWithViewport(clearParam, tex.mipmap);
}
bool DrawContext::hasFBOCtx()
{
    return fboCtxStack.empty();
}
bool DrawContext::hasNotFBOCtx()
{
    return !fboCtxStack.empty();
}
void DrawContext::pushFBOCtx(const FBOContext& fboCtx)
{
    fboCtxStack.emplace_back(fboCtx);
}
void DrawContext::popFBOCtx()
{
    fboCtxStack.pop_back();
}
const FBOContext& DrawContext::topFBOCtx()
{
    return fboCtxStack.back();
}
} // namespace Draw
} // namespace Voxol::Render
