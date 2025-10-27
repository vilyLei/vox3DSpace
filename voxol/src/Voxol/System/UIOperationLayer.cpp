#include "UIOperationLayer.h"
namespace Voxol::System
{
namespace Mouse
{

void MouseCtroller::selectWithSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
{
    if (selectType != SelectType::Single)
    {
        return;
    }
    auto bvh = targetSys->bvh;
    if (evt.isMoving() || evt.isBegin())
    {
        qeIds.clear();
        bvh->queryPoint(evt.globalPos, qeIds);
    }

    free           = qeIds.empty();

    auto etStorage = targetSys->entityStorage->comp;

    auto topId = qeIds.empty() ? Render::Component::INVALID_ID : qeIds.back();

    if (evt.isBegin())
    {
        dragging = false;
    }

    if (topId != Render::Component::INVALID_ID && evt.isBegin())
    {
        etId          = topId;
        originEtPos   = etStorage->getEntityXYAt(etId);
        unitTransform = etStorage->getEntityTransformAt(etId);
        return;
    }

    if (etId != Render::Component::INVALID_ID && evt.isDragging())
    {
        auto id = etId;
        auto pv = originEtPos;

        pv += offset;
        etStorage->setEntityXYAt(pv, id);
        auto b0 = bvh->getBoundsAt(id);
        auto b1 = b0;

        // move out
        dirtyCall(b0, 0);
        b1.moveTo(pv.x, pv.y);
        // move in
        dirtyCall(b1, 1);

        bvh->updateItemBoundsByObjectId(id, b1);
        bvh->updateDirty();
        dragging = true;
        return;
    }
    if (evt.isEnd())
    {
        if (etId != Render::Component::INVALID_ID && dragging)
        {
            dragging = false;
            etStorage->historyManager->pushItem({unitTransform, etId});
        }
        etId = Render::Component::INVALID_ID;
        return;
    }

    if (evt.isDragging() && etId == Render::Component::INVALID_ID)
    {
        selectionBounds.toEmpty(evt.originGlobalPos);
        selectType = SelectType::Bounds;
    }
}

void MouseCtroller::selectWithBounds(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
{
    if (selectType != SelectType::Bounds)
    {
        return;
    }

    selectionBounds.toEmpty(evt.originGlobalPos);
    selectionBounds.addXY(evt.globalPos);

    
    auto bvh = targetSys->bvh;

    if (evt.isMoving() || evt.isBegin())
    {
        qeIds.clear();
        bvh->queryBounds(selectionBounds, qeIds);
    }
    free = qeIds.empty();
    if (evt.isEnd())
    {
        printf("MouseCtroller::selectBtnBounds() end().\n");
        etId       = Render::Component::INVALID_ID;
        selectType = SelectType::Single;
    }
}
void MouseCtroller::upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param)
{
    handler.upateMouseLeftBtnParam(rctx, param, [&, this](const System::Mouse::MouseEvent& evt, const Math::Vec2& offset) {
        if (selectType == SelectType::Single)
        {
            selectWithSingle(evt, offset);
            return;
        }
        if (selectType == SelectType::Bounds)
        {
            selectWithBounds(evt, offset);
            return;
        }
    });
}
}
void UIOperationLayer::initialize()
{
    mouseCtrl.targetSys = etSceneSys;
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