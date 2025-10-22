#ifndef VOXOL_UI_OPERATION_LAYER_H
#define VOXOL_UI_OPERATION_LAYER_H

#include "../Base/BaseDefine.h"
#include "../Tile/TileSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../System/MouseEventSystem.h"
#include "../System/ShortcutManager.h"

namespace Voxol::System
{
namespace Mouse
{
enum class SelectType
{
    Single,
    Multiple,
    Bounds
};
struct MouseCtroller
{
    Math::Vec2           originEtPos{};
    int32_t              etId = -1;
    std::vector<int32_t> qeIds{};

    Render::Component::UnitTransform unitTransform{};

    Tile::TileSystem::SP           tileSys;
    Render::EntityRenderSystem::SP targetSys;
    Math::Bounds                   selectBounds{};
    SelectType                     selectType = SelectType::Single;

    System::Mouse::MouseEvtHandler handler{};

    bool dragging = false;

    void selectSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
    {
        auto bvh = targetSys->bvh;
        if (evt.isMoving() || evt.isBegin())
        {
            qeIds.clear();
            bvh->queryPoint(evt.globalPos, qeIds);
        }
        auto etStorage = targetSys->entityStorage->comp;

        int32_t topId = qeIds.empty() ? -1 : qeIds.back();

        if (evt.isBegin()) {
            dragging = false;
        }

        if (topId >= 0 && evt.isBegin())
        {
            etId          = topId;
            originEtPos   = etStorage->getEntityXYAt(etId);
            unitTransform = etStorage->getEntityTransformAt(etId);
            return;
        }

        if (etId >= 0 && evt.isDragging())
        {
            auto id = etId;
            auto pv = originEtPos;

            pv += offset;
            etStorage->setEntityXYAt(pv, id);
            auto b0 = bvh->getBoundsAt(id);
            auto b1 = b0;

            // move out
            tileSys->addDirtyBounds(b0, 0);
            b1.moveTo(pv.x, pv.y);
            // move in
            tileSys->addDirtyBounds(b1, 1);

            bvh->updateItemBoundsByObjectId(id, b1);
            bvh->updateDirty();
            dragging = true;
            return;
        }
        if (evt.isEnd())
        {
            if (etId >= 0 && dragging)
            {
                dragging = false;
                etStorage->historyManager->pushItem({unitTransform, etId});
            }
            etId = -1;
        }
    }

    void upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param)
    {
        handler.upateMouseLeftBtnParam(rctx, param, [&, this](const System::Mouse::MouseEvent& evt, const Math::Vec2& offset) {
            selectSingle(evt, offset);
        });
    }
};
} // namespace Mouse


class UIOperationLayer
{
public:
    UIOperationLayer()  = default;
    ~UIOperationLayer() = default;

public:
    Tile::TileSystem::SP           tileSys;
    Render::EntityRenderSystem::SP etRenderSys;

    Mouse::MouseCtroller           mouseCtrl;
    ShortcutManager shortcutMana{};

public:
    void initialize();

    void updateKeyboardParams(int key, int scancode, int action, int mods);
    void updateMouseParams(const Render::Draw::DrawContext& ctx, const Mouse::MouseInputParam& param);
};

} // namespace Voxol::System
#endif