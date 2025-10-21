
#include "UIMouseCtrl.h"
namespace Voxol::System
{
bool UIMouseCtrl::setMouseParams(Motion::ViewComponent& view, const Mouse::MouseInputParam& param)
{
    auto  viewDirty = false;
    auto& mousePos  = view.mousePos;
    mousePos        = {param.x, param.y};
    //printf("UIMouseCtrl::setMouseParams(), param.type: %d, param.value: %f\n", param.type, param.value);
    switch (param.type)
    {
        case Mouse::MouseEventType::MouseMiddleDown:
        {
            view.moveBegin(mousePos);
        }
        break;
        case Mouse::MouseEventType::MouseMiddleUp:
        {
            view.moveEnd(mousePos);
        }
        break;
        case Mouse::MouseEventType::MouseMiddleMove:
        {

            viewDirty = view.move(mousePos);
        }
        break;
        case Mouse::MouseEventType::MouseScroll:
        {
            //#ifdef _WIN32
            //auto dv   = std::abs(param.value) >= 5 ? param.value * 0.01f : param.value;
            auto dv   = param.value > 0.0f ? 1.0f: -1.0f;
            //printf("scroll dv: %f, param.value: %f\n", dv, param.value);
            viewDirty = view.updateViewZoom(mousePos, dv, 1.2f);
        }
        break;
        case Mouse::MouseEventType::MouseMiddleClick:
        {

            /*
            printf("UIMouseCtrl::setMouseParams(), click, type: %d, value: %f\n", type, value);
            printf("UIMouseCtrl::setMouseParams(), click, mousePos(%f, %f)\n", mousePos.x, mousePos.y);
            auto  i      = 0;
            auto  pos    = mousePos;
            auto& node   = cmdBatchNodes[i];
            auto& bounds = node.drcDesc.bounds;
            printf("UIMouseCtrl::setMouseParams(), click, node(x=%f, y=%f), scale(sx=%f, sy=%f)\n", bounds.pos.x, bounds.pos.y, bounds.width, bounds.height);
            Mat33 mat{};
            node.updateToMat33(mat);
            printf("mat:\n");
            mat.print();
            Mat33 invMat{};
            mat.inverseTo(invMat);
            printf("invMat:\n");
            invMat.print();

            auto&& v0 = mat.mapPoint({0.0f, 0.0f});
            printf("UIMouseCtrl::setMouseParams(), click, v0(%f, %f)\n", v0.x, v0.y);
            auto&& v1  = invMat.mapPoint({mousePos.x, mousePos.y});
            auto   hit = node.contains(v1.x, v1.y);
            printf("UIMouseCtrl::setMouseParams(), click, node.contains() hit: %d\n", hit);
            v1.x *= bounds.width;
            v1.y *= bounds.height;
            printf("UIMouseCtrl::setMouseParams(), click, v1(%f, %f)\n", v1.x, v1.y);
            //*/
        }

        default:
            break;
    }
    if (viewDirty)
    {
        view.update();
    }
    // if (type == 4)
    // {
    //     dirty = true;
    //     canvas.view.updateViewZoom(mousePos, flag / -100.0f, 1.2f);
    //     auto& viewDesc = canvas.view.desc;
    //     auto& pos      = viewDesc.position;
    //     viewMat.setTo(pos.x, pos.y, viewDesc.zoom, viewDesc.zoom);
    // }
    return viewDirty;
}
} // namespace Voxol::Motion