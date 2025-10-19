#include "MouseEventSystem.h"

namespace Voxol::System
{
namespace Mouse
{
void EventManager::upateMouseParam(const Render::Draw::DrawContext& rctx, const System::UIMouseParam& param, const TargetSysParam& sys)
{

    Math::Vec2 mousePos{param.x, param.y};
    auto&      drawParam = rctx.drawParam;
    auto&&     wpv       = drawParam.invViewMat.mapPoint(mousePos);
    if (param.type == System::UIMouseType::MOUSE_MOVE || param.type == System::UIMouseType::MOUSE_DOWN)
    {
        queryEIds.clear();
        if (drawParam.viewVBounds.contains(mousePos) && sys.bvh)
        {
            sys.bvh->queryPoint(wpv, queryEIds);
        }
    }

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
                dragEvt.targetId       = topId;
                dragEvt.mouseOriginPos = wpv;
                if (sys.storage)
                {
                    dragEvt.originTransform = sys.storage->getEntityTransformAt(topId);
                    dragEvt.entityOriginPos = sys.storage->getEntityXYAt(topId);
                }
                dragEvt.begin();
            }
        }
        break;
        /// mouse up
        case System::UIMouseType::MOUSE_UP:
        {
            if (dragEvt.isDragging())
            {
                auto id = dragEvt.targetId;

                if (sys.storage)
                {
                    sys.storage->historyManager->pushItem({dragEvt.originTransform, id});
                }
            }
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
                if (sys.storage)
                {
                    //auto itemTrans = sys.storage->getEntityTransformAt(id);
                    //sys.storage->historyManager->pushItem({itemTrans, id});

                    sys.storage->setEntityXYAt(pv, id);
                    auto b0 = sys.bvh->getBoundsAt(id);
                    auto b1 = b0;
                    if (sys.tileSys)
                    {
                        // ÒÆ³ö
                        sys.tileSys->addDirtyBounds(b0, 0);
                        b1.moveTo(pv.x, pv.y);
                        // ÒÆÈë
                        sys.tileSys->addDirtyBounds(b1, 1);
                    }
                    if (sys.bvh)
                    {
                        sys.bvh->updateItemBoundsByObjectId(id, b1);
                        sys.bvh->updateDirty();
                    }
                }
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
} // namespace Mouse
} // namespace Voxol::System