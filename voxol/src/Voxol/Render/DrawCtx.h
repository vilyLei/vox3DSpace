#ifndef VOXOL_RENDER_DRAW_CTX_H
#define VOXOL_RENDER_DRAW_CTX_H

#include <functional>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"

namespace Voxol::Render
{
namespace Draw
{
using DrawCallType = std::function<void(const Math::VxRect& viewWBounds, const Math::Mat33& vpMat)>;

struct Viewport
{
    int x, y, width, height;
};
struct ClearColor
{
    float r = 1, g = 1, b = 1, a = 1;
};


struct DrawParams
{
    Math::Mat33  viewMat{};
    Math::Mat33  projMat{};
    Math::VxRect viewVBounds{};
    Math::VxRect viewWBounds{};
};
struct DrawContext
{
    Viewport viewport{};
    ClearColor clearColor{};
    
    DrawParams   params{};
    DrawCallType drawCall;
    float        zoom = 1;
    bool        dirty = 1;
};
} // namespace Draw
} // namespace Voxol::Render
#endif