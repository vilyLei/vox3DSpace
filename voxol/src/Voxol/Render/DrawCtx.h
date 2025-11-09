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

struct FBOCtxNode
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

    GLuint getRTTextureAt(int index) const;
};
struct FBOCtxStack
{
    uint32_t                depth = 0;
    std::vector<OglFbo::SP> freeFboStack;
    std::vector<FBOCtxNode> ctxStack;
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
    void              pushNode(const FBOCtxNode& fboCtx, bool autoRenderBegin = true) const;
    void              popNode() const;
    const FBOCtxNode& topNode() const;
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