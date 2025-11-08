#ifndef VOXOL_RENDER_DRAW_PARAM_H
#define VOXOL_RENDER_DRAW_PARAM_H

#include <functional>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Render
{
namespace Draw
{
using DrawCallType  = std::function<void(const Math::VxRect& bounds, const Math::Mat33& vpMat)>;
using DrawQueryType = std::function<int(const Math::VxRect& bounds, int phase)>;

struct Viewport
{
    int x, y, width, height;
};
struct ClearColor
{
    float r = 1, g = 1, b = 1, a = 1;
};

struct ClearParams
{
    Viewport   viewport{};
    ClearColor clearColor{};
    GLbitfield clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    void       applyViewport() const;
    void       applyClearColor() const;
    void       apply() const;
};

struct OglTextureUnit
{
    uint32_t index   = 0;
    uint32_t width   = 0;
    uint32_t height  = 0;
    GLuint   texture = GL_ZERO;
    bool     mipmap  = false;
};
struct DrawParams
{
    Math::Mat33  viewMat{};
    Math::Mat33  invViewMat{};
    Math::Mat33  projMat{};
    Math::VxRect viewVBounds{};
    Math::VxRect viewWBounds{};
};
} // namespace Draw
} // namespace Voxol::Render
#endif