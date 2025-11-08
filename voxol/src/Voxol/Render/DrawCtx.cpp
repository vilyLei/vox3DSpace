#include "DrawCtx.h"
namespace Voxol::Render
{
namespace Draw
{

void DrawContext::bindFBOCtx()
{
    auto fctx = fboCtxStack.back();
    fctx.fbo->bindFBO();
}
void DrawContext::renderBeginWithFBOCtx()
{
    auto fctx = fboCtxStack.back();
    fctx.fbo->bindFBO();
    auto  texIndex = 0;
    auto& tex      = fctx.textures[texIndex];
    fctx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
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
        //fctx.fbo->unbindFBO(preFCtx.clearParam, tex.mipmap);
        fctx.fbo->unbindFBOWithViewport(preFCtx.clearParam, tex.mipmap);
        preFCtx.fbo->bindFBO();
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
}
} // namespace Voxol::Render
