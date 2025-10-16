#ifndef VOXOL_MOUSE_EVENT_SYSTEM_H
#define VOXOL_MOUSE_EVENT_SYSTEM_H

#include "UIMouseCtrl.h"
#include "../Render/DrawCtx.h"
#include "../Render/EntityRenderSystem.h"
namespace Voxol::System
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
    void                 upateMouseParam(const Render::Draw::DrawContext& rctx, Render::EntitySysBVH& bvh, Render::EntityCompStorage& storage, const System::UIMouseParam& param)
    {

        Math::Vec2 mousePos{param.x, param.y};
        auto&      drawParam = rctx.drawParam;
        auto&&     wpv       = drawParam.invViewMat.mapPoint(mousePos);
        if (param.type == System::UIMouseType::MOUSE_MOVE)
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
            case System::UIMouseType::MOUSE_DOWN:
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
            case System::UIMouseType::MOUSE_UP:
            {
                dragEvt.end();
            }
            break;
            /// mouse move
            case System::UIMouseType::MOUSE_MOVE:
            {
                if (dragEvt.isBegin() || dragEvt.isDragging())
                {
                    dragEvt.drag();

                    auto id = dragEvt.targetId;
                    auto pv = dragEvt.entityOriginPos;

                    auto dv = wpv - dragEvt.mouseOriginPos;
                    pv.x += dv.x;
                    pv.y += dv.y;
                    storage.setEntityXYAt(pv, id);
                    auto b = bvh.getBoundsAt(id);
                    b.setXY(pv.x, pv.y);
                    bvh.updateItemBoundsByObjectId(id, b);
                    bvh.updateDirty();
                }
            }
            break;
            /// mouse scroll
            case System::UIMouseType::MOUSE_SCROLL:
            {
            }
            break;
            /// mouse click
            case System::UIMouseType::MOUSE_CLICK:
            {
            }
            break;

            default:
                break;
        }
    }
};
} // namespace Mouse


}
#endif