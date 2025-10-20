#ifndef VOXOL_MOUSE_EVENT_SYSTEM_H
#define VOXOL_MOUSE_EVENT_SYSTEM_H

#include "UIMouseCtrl.h"
#include "../Render/DrawCtx.h"
#include "../Render/EntityRenderSystem.h"
#include "../Tile/TileSystem.h"
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

    Render::Component::UnitTransform originTransform{};
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
struct TargetSysParam
{
    Tile::TileSystem::SP tileSys{};
    Render::EntitySysBVH::SP bvh{};
    Render::EntityCompStorage::SP storage{};
};
struct EventManager
{
    DragEvent            dragEvt{};
    std::vector<int32_t> queryEIds{};
    void                 upateMouseParam(const Render::Draw::DrawContext& rctx, const System::UIMouseParam& param, const TargetSysParam& sys);
    
};

using MouseInputParam = System::UIMouseParam;

struct MouseEvent
{
    // 0: begin, 1: dragging moving, 2: end
    UIMouseActionPhase phase = UIMouseActionPhase::End;
    UIMouseType type  = UIMouseType::MOUSE_MOVE;

    bool       dirty = false;
    Math::Vec2 originPos{};
    Math::Vec2 localPos{};
    Math::Vec2 globalPos{};

    bool isBegin() const
    {
        return phase == UIMouseActionPhase::Begin;
    }
    bool isEnd() const
    {
        return phase == UIMouseActionPhase::End;
    }
    bool isDragging() const
    {
        return phase == UIMouseActionPhase::Dragging;
    }
    void begin()
    {
        phase = UIMouseActionPhase::Begin;
        dirty = false;
    }
    void drag()
    {
        phase = UIMouseActionPhase::Dragging;
        dirty = true;
    }
    void end()
    {
        phase = UIMouseActionPhase::End;
        dirty = false;
    }
    void resetState()
    {
        dirty = false;
    }
};
using MouseCallType = std::function<void(const MouseEvent& evt, const Math::Vec2& dv)>;
struct MouseEvtHandler
{
    MouseEvent evt{};
    void       upateMouseParam(const Render::Draw::DrawContext& rctx, const MouseInputParam& param, const MouseCallType& callback)
    {

        Math::Vec2 mousePos{param.x, param.y};
        auto&&     wpv = rctx.drawParam.invViewMat.mapPoint(mousePos);
        evt.localPos   = mousePos;
        evt.globalPos  = wpv;
        evt.type       = param.type;
        switch (param.type)
        {
            /// mouse down
            case UIMouseType::MOUSE_DOWN:
            {
                if (!evt.isBegin())
                {
                    evt.begin();
                    evt.originPos = wpv;
                    callback(evt, {});
                }
            }
            break;
            /// mouse up
            case UIMouseType::MOUSE_UP:
            {
                if (evt.isBegin() || evt.isDragging())
                {
                    evt.end();
                    callback(evt, {});
                }
            }
            break;
            /// mouse move
            case UIMouseType::MOUSE_MOVE:
            {
                if (evt.isBegin() || evt.isDragging())
                {
                    evt.drag();

                    auto&& dv = wpv - evt.originPos;

                    callback(evt, dv);
                }
            }
            break;
            /// mouse scroll
            case UIMouseType::MOUSE_SCROLL:
            {
            }
            break;
            /// mouse click
            case UIMouseType::MOUSE_CLICK:
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