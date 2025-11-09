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

namespace Voxol::Render
{
namespace Draw
{

struct FBOCtxNode
{
    Math::Mat33                         viewMat;
    mutable OglFbo::SP                  fbo;
    ClearParams                         clearParam{};
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

//struct FBOGraphNodeGuard
//{
//    using GuardType = decltype(Base::Scope::make_scope_enter_and_exit_guard(
//        std::declval<std::function<void() noexcept>>(),
//        std::declval<std::function<void() noexcept>>()));
//    GuardType       scopeGuard;
//    FBORenderGraph& graph;
//};

template <typename GuardT>
struct FBOGraphNodeGuard
{
    GuardT scopeGuard;
    FBORenderGraph& graph;

    FBOGraphNodeGuard(const FBOGraphNodeGuard&)            = delete;
    FBOGraphNodeGuard& operator=(const FBOGraphNodeGuard&) = delete;

    FBOGraphNodeGuard(GuardT&& g, FBORenderGraph& gr) noexcept
        :
        scopeGuard(std::move(g)), graph(gr)
    {
    }
    FBOGraphNodeGuard(FBOGraphNodeGuard&& other) noexcept
        :
        scopeGuard(std::move(other.scopeGuard)), graph(other.graph)
    {}
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


    //[[nodiscard]] auto makeFBOGraphNodeGuard(const Render::Draw::FBOCtxNode& fboCtx, const std::string& debugEnterInfo, const std::string& debugExitInfo) const;

    GLuint getRTTextureAt(int index) const;

    [[nodiscard]] auto makeFBOGraphNodeGuard(const Render::Draw::FBOCtxNode& fboCtx, const std::string& debugEnterInfo, const std::string& debugExitInfo) const
    {
        /*
        return Base::Scope::make_scope_enter_and_exit_guard(
            [&, this]() noexcept {
                printf("%s\n", debugEnterInfo.c_str());
                printf("makeFBOGraphNode exec enter graph.pushNode() ...\n");
                fboGraph.pushNode(fboCtx);
            },
            [&, this]() noexcept {
                fboGraph.popNode();
                printf("makeFBOGraphNode exec exit graph.popNode() ...\n");
                printf("%s\n", debugExitInfo.c_str());
            });
        //*/
        /*
        return FBOGraphNodeGuard{ Base::Scope::make_scope_enter_and_exit_guard(
            [&, this]() noexcept {
                printf("%s\n", debugEnterInfo.c_str());
                printf("makeFBOGraphNode exec enter graph.pushNode() ...\n");
                fboGraph.pushNode(fboCtx);
            },
            [&, this]() noexcept {
                fboGraph.popNode();
                printf("makeFBOGraphNode exec exit graph.popNode() ...\n");
                printf("%s\n", debugExitInfo.c_str());
                                     }),
                                 fboGraph};
        //*/
        // 推断出 Guard 的真实类型
        auto&& guard = Base::Scope::make_scope_enter_and_exit_guard(
            [&, this]() noexcept {
                printf("%s\n", debugEnterInfo.c_str());
                printf("makeFBOGraphNode exec enter graph.pushNode() ...\n");
                fboGraph.pushNode(fboCtx);
            },
            [&, this]() noexcept {
                fboGraph.popNode();
                printf("makeFBOGraphNode exec exit graph.popNode() ...\n");
                printf("%s\n", debugExitInfo.c_str());
            });

        // Guard 的实际类型是 decltype(guard)
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