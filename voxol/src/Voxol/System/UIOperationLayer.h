#ifndef VOXOL_UI_OPERATION_LAYER_H
#define VOXOL_UI_OPERATION_LAYER_H

#include "../Base/BaseDefine.h"
#include "../Render/EntitySceneSystem.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../System/MouseEventSystem.h"
#include "../System/ShortcutManager.h"
#include <functional>
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

using InputDirtyCallType = std::function<void(const Math::Bounds& bounds, uint32_t type, uint32_t etId)>;
struct MouseController
{
    Math::Vec2           originEtPos{};
    uint32_t             etId = Render::Component::INVALID_ID;
    std::vector<uint32_t> qeIds{};

    Render::Component::UnitTransform unitTransform{};

    InputDirtyCallType             dirtyCall;
    Render::EntitySceneSystem::SP targetSys;
    Math::Bounds                   selectionBounds{};
    SelectType                     selectType = SelectType::Single;

    System::Mouse::MouseEvtHandler handler{};

    bool dragging = false;
    // the input ctrl task flow is free or block 
    bool free     = true;
    bool isBoundsSelection() const
    {
        return selectType == SelectType::Bounds;
    }
    bool isSingleSelection() const
    {
        return selectType == SelectType::Single;
    }
    void selectWithSingle(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset);
    void selectWithBounds(const System::Mouse::MouseEvent& evt, const Math::Vec2& offset);

    void upateLeftMouseParam(const Render::Draw::DrawContext& rctx, const System::Mouse::MouseInputParam& param);
};
} // namespace Mouse


class UIOperationLayer
{
public:
    UIOperationLayer()  = default;
    ~UIOperationLayer() = default;

public:
    Render::EntitySceneSystem::SP etSceneSys;

    Mouse::MouseController mouseCtrl;
    ShortcutManager shortcutMana{};

public:
    void initialize();

    void updateKeyboardParams(int key, int scancode, int action, int mods);
    void updateMouseParams(const Render::Draw::DrawContext& ctx, const Mouse::MouseInputParam& param);
};

} // namespace Voxol::System
#endif