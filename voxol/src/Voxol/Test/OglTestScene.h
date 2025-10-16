#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Tile/TileSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglFbo.h"
#include "OglResUtils.h"
#include "OglImage.h"
#include "OglText.h"
#include "../Motion/UIMouseCtrl.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{
namespace Mouse
{
struct DragEvent
{
    int32_t targetId = -1;
    // 0: begin, 1: dragging moving, 2: end
    int32_t    phase = 2;
    bool       dirty = false;
    Math::Vec2 mouseOriginPos{};
    Math::Vec2 mousePos{};
    Math::Vec2 entityOriginPos{};

    bool isBegin()
    {
        return phase == 0;
    }
    bool isEnd()
    {
        return phase == 2;
    }
    bool isDragging()
    {
        return phase == 1;
    }
    void begin()
    {
        phase = 0;
    }
    void drag()
    {
        phase = 1;
        dirty = true;
    }
    void end()
    {
        phase    = 2;
        targetId = -1;
        dirty    = false;
    }
    void resetState()
    {
        dirty = false;
    }
};

struct EventManager
{
    DragEvent            dragEvt{};
    std::vector<int32_t> queryEIds{};
    void                 upateMouseParam(const Render::Draw::DrawContext& rctx, Render::EntitySysBVH& bvh, Render::EntityCompStorage& storage, const Motion::UIMouseParam& param)
    {

        Math::Vec2 mousePos{param.x, param.y};
        auto&      drawParam = rctx.drawParam;
        auto&&     wpv       = drawParam.invViewMat.mapPoint(mousePos);
        if (param.type == Motion::UIMouseType::MOUSE_MOVE)
        {
            queryEIds.clear();
            if (drawParam.viewVBounds.contains(mousePos))
            {
                bvh.queryPoint(wpv, queryEIds);
            }
        }
        //dragEvt
        auto topId = queryEIds.empty() ? -1 : queryEIds.back();
        if (dragEvt.isDragging())
        {
            queryEIds.clear();
        }
        switch (param.type)
        {
            /// mouse down
            case Motion::UIMouseType::MOUSE_DOWN:
            {
                if (topId >= 0)
                {
                    dragEvt.targetId        = topId;
                    dragEvt.mouseOriginPos  = wpv;
                    dragEvt.entityOriginPos = storage.getEntityXYAt(topId);
                    dragEvt.begin();
                }
            }
            break;
            /// mouse up
            case Motion::UIMouseType::MOUSE_UP:
            {
                dragEvt.end();
            }
            break;
            /// mouse move
            case Motion::UIMouseType::MOUSE_MOVE:
            {
                if (dragEvt.isBegin() || dragEvt.isDragging())
                {
                    dragEvt.drag();

                    auto id = dragEvt.targetId;
                    auto pv = dragEvt.entityOriginPos;

                    auto dv = wpv - dragEvt.mouseOriginPos;
                    // {wpv.x - dragEvt.mouseOriginPos.x, wpv.y - dragEvt.mouseOriginPos.y};
                    pv.x += dv.x;
                    pv.y += dv.y;
                    //storage.setEntityXYAt(pv, id);
                    auto b = bvh.getBoundsAt(id);
                    b.setXY(pv.x, pv.y);
                    bvh.updateItemBoundsByObjectId(id, b);
                    bvh.updateDirty();
                }
            }
            break;
            /// mouse scroll
            case Motion::UIMouseType::MOUSE_SCROLL:
            {
            }
            break;
            /// mouse click
            case Motion::UIMouseType::MOUSE_CLICK:
            {
            }
            break;

            default:
                break;
        }
    }
};
} // namespace Mouse
//Mouse::EventManager mouseMana{};
//void                   setMouseParams(float x, float y, int type, float value)
//{
//    mouseMana.upateMouseParam(rctx, unitScene, {x, y, type, value});
//}
//}

class OglTestScene
{

public:
    OglTestScene()          = default;
    virtual ~OglTestScene() = default;

public:
    void                      initScene();
    void                      render(const Voxol::Math::Mat33& vpMat);
    void                      setMouseParams(const Motion::UIMouseParam& param);
    Render::Draw::DrawContext drawCtx{};
    Mouse::EventManager       mouseEvtMana{};

private:
    void initVoassScene();
    void renderVoass(const Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;

    Render::OglFbo             mFbo{};
    Tile::TileSystem           tileSys{};
    Render::EntityRenderSystem etRenderSys{};

    Gpu::DrawingUnit              baseDrawUnit{};
    Gpu::DrawingUnit              texDrawUnit{};
    Gpu::DrawingUnit              redFormatexDrawUnit{};
    Gpu::DrawingUnit              glyphDrawUnit{};
    Gpu::DrawingUnit              pngUnit{};
    MSDFText                      msdfText{};
    std::vector<Gpu::DrawingUnit> msdfTextDrawUnits{};

    std::vector<Gpu::DrawingUnit> sdfDrawUnits{8};

    Gpu::DrawingUnit     tile0Unit{};
    Gpu::DrawingUnit     boundsUnit{};
    //std::vector<int32_t> queriedEIds{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
