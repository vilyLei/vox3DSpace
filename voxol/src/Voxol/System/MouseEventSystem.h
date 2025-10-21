#ifndef VOXOL_MOUSE_EVENT_SYSTEM_H
#define VOXOL_MOUSE_EVENT_SYSTEM_H

#include "../Render/DrawCtx.h"
#include "../Render/EntityRenderSystem.h"
#include "../Tile/TileSystem.h"
namespace Voxol::System
{
namespace Mouse
{
enum class MouseActionPhase : unsigned char
{
    Begin    = 0,
    Dragging = 1,
    Moving   = Dragging,
    End      = 2
};
enum class MouseEventType : char
{
    MouseDown,
    MouseUp,
    MouseMove,

    MouseMiddleDown,
    MouseMiddleUp,
    MouseMiddleMove,

    MouseRightDown,
    MouseRightUp,
    MouseRightMove,

    MouseScroll,
    MouseClick
};


struct MouseInputParam
{
    float x, y;

    MouseEventType type = MouseEventType::MouseMove;

    float value;
};
struct MouseEvent
{
    // 0: begin, 1: dragging moving, 2: end
    MouseActionPhase phase    = MouseActionPhase::End;
    MouseEventType type     = MouseEventType::MouseMove;
    bool           dirty    = false;
    bool           moving   = false;
    bool           dragging = false;
    Math::Vec2     originPos{};
    Math::Vec2     localPos{};
    Math::Vec2     globalPos{};

    bool isBegin() const
    {
        return phase == MouseActionPhase::Begin;
    }
    bool isEnd() const
    {
        return phase == MouseActionPhase::End;
    }
    bool isDragging() const
    {
        return phase == MouseActionPhase::Dragging && dragging;
    }
    bool isMoving() const
    {
        return phase == MouseActionPhase::Moving && moving;
    }
    void begin()
    {
        phase    = MouseActionPhase::Begin;
        dirty    = false;
        moving   = false;
        dragging = false;
    }
    void drag()
    {
        phase    = MouseActionPhase::Dragging;
        dragging = true;
        dirty    = true;
    }
    void move()
    {
        phase  = MouseActionPhase::Moving;
        moving = true;
        dirty  = true;
    }
    void end()
    {
        phase    = MouseActionPhase::End;
        dirty    = false;
        moving   = false;
        dragging = false;
    }
    void resetState()
    {
        dirty = false;
    }
};
using MouseCallType = std::function<void(const MouseEvent& evt, const Math::Vec2& offset)>;
struct MouseEvtHandler
{
    MouseEvent evt{};
    void       upateMouseLeftBtnParam(const Render::Draw::DrawContext& rctx, const MouseInputParam& param, const MouseCallType& callback)
    {
        evt.localPos  = {param.x, param.y};
        evt.globalPos = rctx.drawParam.invViewMat.mapPoint(evt.localPos);
        evt.type      = param.type;
        switch (param.type)
        {
            /// mouse down
            case Mouse::MouseEventType::MouseDown:
            {
                if (!evt.isBegin())
                {
                    evt.begin();
                    evt.originPos = evt.globalPos;
                    callback(evt, {});
                }
            }
            break;
            /// mouse up
            case Mouse::MouseEventType::MouseUp:
            {
                if (evt.isBegin() || evt.isDragging())
                {
                    evt.end();
                    callback(evt, {});
                }
            }
            break;
            /// mouse move
            case Mouse::MouseEventType::MouseMove:
            {
                auto flag = evt.isBegin();
                evt.move();
                if (flag || evt.isDragging())
                {
                    evt.drag();
                    auto&& dv = evt.globalPos - evt.originPos;

                    callback(evt, dv);
                }
                else
                {
                    callback(evt, {});
                }
            }
            break;
            default:
                break;
        }
    }

    void upateMouseRightBtnParam(const Render::Draw::DrawContext& rctx, const MouseInputParam& param, const MouseCallType& callback)
    {
        evt.localPos  = {param.x, param.y};
        evt.globalPos = rctx.drawParam.invViewMat.mapPoint(evt.localPos);
        evt.type      = param.type;
        switch (param.type)
        {
            /// mouse down
            case Mouse::MouseEventType::MouseRightDown:
            {
                if (!evt.isBegin())
                {
                    evt.begin();
                    evt.originPos = evt.globalPos;
                    callback(evt, {});
                }
            }
            break;
            /// mouse up
            case Mouse::MouseEventType::MouseRightUp:
            {
                if (evt.isBegin() || evt.isDragging())
                {
                    evt.end();
                    callback(evt, {});
                }
            }
            break;
            /// mouse move
            case Mouse::MouseEventType::MouseRightMove:
            {
                auto flag = evt.isBegin();
                evt.move();
                if (flag || evt.isDragging())
                {
                    evt.drag();
                    auto&& dv = evt.globalPos - evt.originPos;

                    callback(evt, dv);
                }
                else
                {
                    callback(evt, {});
                }
            }
            break;
            default:
                break;
        }
    }
    void upateMouseMiddleBtnParam(const Render::Draw::DrawContext& rctx, const MouseInputParam& param, const MouseCallType& callback)
    {
        evt.localPos  = {param.x, param.y};
        evt.globalPos = rctx.drawParam.invViewMat.mapPoint(evt.localPos);
        evt.type      = param.type;
        switch (param.type)
        {
            /// mouse down
            case Mouse::MouseEventType::MouseMiddleDown:
            {
                if (!evt.isBegin())
                {
                    evt.begin();
                    evt.originPos = evt.globalPos;
                    callback(evt, {});
                }
            }
            break;
            /// mouse up
            case Mouse::MouseEventType::MouseMiddleUp:
            {
                if (evt.isBegin() || evt.isDragging())
                {
                    evt.end();
                    callback(evt, {});
                }
            }
            break;
            /// mouse move
            case Mouse::MouseEventType::MouseMiddleMove:
            {
                auto flag = evt.isBegin();
                evt.move();
                if (flag || evt.isDragging())
                {
                    evt.drag();
                    auto&& dv = evt.globalPos - evt.originPos;

                    callback(evt, dv);
                }
                else
                {
                    callback(evt, {});
                }
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