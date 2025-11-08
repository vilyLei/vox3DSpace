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
    OglFbo::SP fbo;
    ClearParams clearParam{};
    std::vector<OglTextureUnit> textures;
};
struct DrawContext
{
    std::vector<FBOContext> fboCtxStack;

    ClearParams  clearParam{};
    DrawParams   drawParam{};

    DrawCallType drawCall;
    DrawQueryType drawQueryCall;

    void bindFBOCtx() {
        auto fctx = fboCtxStack.back();
        fctx.fbo->bindFBO();
    }
    void renderBeginWithFBOCtx()
    {
        auto fctx = fboCtxStack.back();
        fctx.fbo->bindFBO();
        auto texIndex = 0;
        auto& tex      = fctx.textures[texIndex];
        fctx.fbo->bindTextureAt(tex.texture, tex.index, tex.width, tex.height);
        fctx.fbo->renderBegin(fctx.clearParam);
    }

    void renderEndWithFBOCtx()
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
    bool hasFBOCtx() {
        return fboCtxStack.empty();
    }
    bool hasNotFBOCtx()
    {
        return !fboCtxStack.empty();
    }

    void pushFBOCtx(const FBOContext& fboCtx)
    {
        fboCtxStack.emplace_back(fboCtx);
    }
    void popFBOCtx()
    {
        fboCtxStack.pop_back();
    }
    const FBOContext& topFBOCtx()
    {
        return fboCtxStack.back();
    }

    float        zoom = 1;
    bool        dirty = 1;
};
} // namespace Draw
} // namespace Voxol::Render
#endif