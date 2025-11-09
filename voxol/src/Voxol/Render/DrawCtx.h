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
    mutable std::vector<OglTextureUnit> texUnits;

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


struct FBORenderGraph
{
    mutable FBOCtxStack fboCtxStack;
    ClearParams backgroundClearParam;

    void              applyViewport() const;
    void              applyClearColor() const;
    void              applyClearViewport() const;

    void              bindNode() const;
    void              renderBegin() const;
    void              renderEnd() const;
    bool              hasNode() const;
    bool              hasNotNode() const;
    void              pushNode(const FBOContext& fboCtx) const;
    void              popNode() const;
    const FBOContext& topNode() const;
    GLuint            getRTTextureAt(int index) const;
};

struct DrawContext
{
    mutable FBORenderGraph fboGraph;

    ClearParams clearParam{};
    DrawParams  drawParam{};

    DrawCallType  drawCall;
    DrawQueryType drawQueryCall;

    float zoom  = 1;
    bool  dirty = true;
};
} // namespace Draw
} // namespace Voxol::Render
#endif