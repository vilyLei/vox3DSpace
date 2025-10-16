#include "MouseEventSystem.h"

namespace Voxol::System
{
namespace Mouse
{
void EventManager::upateMouseParam(Tile::TileSystem& tileSys, const Render::Draw::DrawContext& rctx, Render::EntitySysBVH& bvh, Render::EntityCompStorage& storage, const System::UIMouseParam& param)
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
                tileSys.addDirtyBounds(b);
                b.setXY(pv.x, pv.y);
                tileSys.addDirtyBounds(b);
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
}
}