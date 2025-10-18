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
    void                 upateMouseParam(Tile::TileSystem& tileSys, const Render::Draw::DrawContext& rctx, Render::EntitySysBVH::SP bvh, Render::EntityCompStorage::SP storage, const System::UIMouseParam& param);
    
};
} // namespace Mouse


}
#endif