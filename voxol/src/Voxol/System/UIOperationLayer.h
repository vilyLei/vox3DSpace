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
    Math::Bounds                   selectionBounds{};
    SelectType                     selectType = SelectType::Single;

    System::Mouse::MouseEvtHandler handler{};

    bool dragging = false;

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