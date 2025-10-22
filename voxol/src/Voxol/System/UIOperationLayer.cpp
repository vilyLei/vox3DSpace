#include "UIOperationLayer.h"
namespace Voxol::System
{
namespace Mouse
{

void MouseCtroller::selectSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
{
    auto bvh = targetSys->bvh;
    if (evt.isMoving() || evt.isBegin())
    {
        qeIds.clear();
        bvh->queryPoint(evt.globalPos, qeIds);
    }
    auto etStorage = targetSys->entityStorage->comp;

    int32_t topId = qeIds.empty() ? -1 : qeIds.back();

    if (evt.isBegin())
    {
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

void MouseCtroller::upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param)
{
    handler.upateMouseLeftBtnParam(rctx, param, [&, this](const System::Mouse::MouseEvent& evt, const Math::Vec2& offset) {
        selectSingle(evt, offset);
    });
}
}
void UIOperationLayer::initialize()
{
    mouseCtrl.tileSys   = tileSys;
    mouseCtrl.targetSys = etRenderSys;
}

void UIOperationLayer::updateKeyboardParams(int key, int scancode, int action, int mods)
{
    shortcutMana.handleKeyEvent(key, scancode, action, mods);
}
void UIOperationLayer::updateMouseParams(const Render::Draw::DrawContext& ctx, const System::Mouse::MouseInputParam& param)
{

    mouseCtrl.upateLeftMouseParam(ctx, param);
}
} // namespace Voxol::System