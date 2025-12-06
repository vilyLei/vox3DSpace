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
#include "../base/ScopeGuard.h"
#include "./Gpu/GpuDrawingDef.h"

namespace Voxol::Render
{
namespace Draw
{

struct FBOCtxNode
{
    Math::Mat33                         viewMat;
    Math::Mat33                         projMat;
    mutable OglFbo::SP                  fbo;
    ClearParams                         clearParam{};
    mutable std::vector<TextureParam>   texUnits;

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
    ClearParams         backgroundClearParam;

    void applyViewport() const;
    void applyClearColor() const;
    void applyClearViewport() const;

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

template <typename GuardT>
struct [[nodiscard]] FBOGraphNodeGuard
{
    GuardT          scopeGuard;
    FBORenderGraph& fboGraph;

    FBOGraphNodeGuard(const FBOGraphNodeGuard&)            = delete;
    FBOGraphNodeGuard& operator=(const FBOGraphNodeGuard&) = delete;

    FBOGraphNodeGuard(GuardT&& g, FBORenderGraph& gr) noexcept
        :
        scopeGuard(std::move(g)), fboGraph(gr)
    {}

    FBOGraphNodeGuard(FBOGraphNodeGuard&& other) noexcept
        :
        scopeGuard(std::move(other.scopeGuard)), fboGraph(other.graph)
    {}

    ~FBOGraphNodeGuard() = default;

    [[nodiscard]] FBORenderGraph*       operator->() noexcept { return &fboGraph; }
    [[nodiscard]] const FBORenderGraph* operator->() const noexcept { return &fboGraph; }

    [[nodiscard]] FBORenderGraph&       operator*() noexcept { return fboGraph; }
    [[nodiscard]] const FBORenderGraph& operator*() const noexcept { return fboGraph; }

    [[nodiscard]] GLuint getRTTextureAt(int index) const noexcept
    {
        return fboGraph.getRTTextureAt(index);
    }

    [[nodiscard]] void execExitFunc() noexcept
    {
        scopeGuard.execExitFunc();
    }
    [[nodiscard]] void resetExitFunc() noexcept { scopeGuard.resetExitFunc(); }
    [[nodiscard]] void dismissExitFunc() noexcept { scopeGuard.dismissExitFunc(); }
};

struct DrawContext
{
    mutable FBORenderGraph fboGraph;

    ClearParams clearParam{};
    DrawParams  drawParam{};

    DrawCallType  drawCall;
    DrawQueryType drawQueryCall;

    mutable Gpu::GPUDrawingStateContext drawStateCtx;

    float zoom  = 1;
    bool  dirty = true;

    GLuint getRTTextureAt(int index) const;

    [[nodiscard]] auto makeFBOGraphNodeGuard(const Render::Draw::FBOCtxNode& fboCtx, const std::string& debugEnterInfo = "", const std::string& debugExitInfo = "") const
    {
        auto&& guard = Base::Scope::make_scope_enter_and_exit_guard(
            [&, this]() noexcept {

                if (!debugEnterInfo.empty())
                    printf("%s\n", debugEnterInfo.c_str());
                //printf("makeFBOGraphNode exec enter graph.pushNode() ...\n");
                fboGraph.pushNode(fboCtx);
            },
            [&, this]() noexcept {
                fboGraph.popNode();
                //printf("makeFBOGraphNode exec exit graph.popNode() ...\n");
                if (!debugExitInfo.empty())
                    printf("%s\n", debugExitInfo.c_str());
            });

        using GuardT = std::decay_t<decltype(guard)>;

        return FBOGraphNodeGuard<GuardT>{std::move(guard), fboGraph};
    }
};

//[[nodiscard]] auto makeFBOGraphNode(const FBORenderGraph& graph, const Render::Draw::FBOCtxNode& fboCtx, const std::string& debugEnterInfo, const std::string& debugExitInfo)
//{
//    return Base::Scope::make_scope_enter_and_exit_guard(
//        [&]() noexcept {
//            printf("%s\n", debugEnterInfo.c_str());
//            printf("makeFBOGraphNode exec enter graph.pushNode() ...\n");
//            graph.pushNode(fboCtx);
//        },
//        [&]() noexcept {
//            graph.popNode();
//            printf("makeFBOGraphNode exec exit graph.popNode() ...\n");
//            printf("%s\n", debugExitInfo.c_str());
//        });
//}
} // namespace Draw
} // namespace Voxol::Render
#endif