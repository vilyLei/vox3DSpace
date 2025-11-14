#include "UIOperationLayer.h"
namespace Voxol::System
{
namespace Mouse
{

void MouseController::selectWithSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
{
    if (selectType != SelectType::Single)
    {
        return;
    }
    auto& bvh = targetSys->bvh;
    if (evt.isMoving() || evt.isBegin())
    {
        qeIds.clear();
        bvh->queryPoint(evt.globalPos, qeIds);
    }

    free = qeIds.empty();

    //printf("qeIds.size(): %lld\n", qeIds.size());
    auto etStorage = targetSys->entityStorage->comp;
    etStorage->checkIds(qeIds);

    auto topId = qeIds.empty() ? Render::ID::INVALID_KEY : qeIds.back();

    if (evt.isBegin() && topId.flags() > 0)
    {
        selectEtId = Render::ID::INVALID_KEY;
        etId = Render::ID::INVALID_KEY;
        return;
    }
    if (evt.isBegin())
    {
        selectEtId = Render::ID::INVALID_KEY;
        dragging = false;
    }

    if (evt.isMoving())
    {
        targetSys->interSrcSys->updateSourceAct(topId, "move");
    }
    else if (evt.isEnd())
    {
        targetSys->interSrcSys->updateSourceAct(topId, "up");
    }

    if (Render::ID::isValidID(topId) && evt.isBegin())
    {
        etId          = topId;
        selectEtId    = etId;
        originEtPos   = etStorage->getEntityGlobalXYAt(etId.protoId());
        unitTransform = etStorage->getEntityTransformAt(etId.protoId());

        targetSys->interSrcSys->updateSourceAct(etId, "down");
        return;
    }


    if (Render::ID::isValidID(etId) && evt.isDragging())
    {
        dirtyCall({}, 0, etId);
        etStorage->setEntityGlobalXYAt(originEtPos + offset, etId.protoId());
        dirtyCall({}, 1, etId);

        dragging = true;
        return;
    }
    if (evt.isEnd())
    {
        if (Render::ID::isValidID(etId) && dragging)
        {
            dragging = false;
            etStorage->historyManager->pushItem({unitTransform, etId});
        }
        etId = Render::ID::INVALID_KEY;
        return;
    }

    if (evt.isDragging() && Render::ID::isInvalidID(etId))
    {
        selectionBounds.toEmpty(evt.originGlobalPos);
        selectType = SelectType::Bounds;
    }
}

void MouseController::selectWithBounds(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset)
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
        etId       = Render::ID::INVALID_KEY;
        selectType = SelectType::Single;
        targetSys->interSrcSys->updateSourceAct(etId, "up");
    }
}
void MouseController::upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param)
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
} // namespace Mouse
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