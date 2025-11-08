#ifndef VOXOL_RENDER_DRAW_CTX_H
#define VOXOL_RENDER_DRAW_CTX_H

#include <functional>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "DrawParam.h"
#include <GL/glew.h>
#include <glfw3.h>
#include <vector>
#include "OglFbo.h"

namespace Voxol::Render
{
namespace Draw
{

struct FBOContext
{

};
struct DrawContext
{
    std::vector<FBOContext> fboCtxStack;

    ClearParams  clearParam{};
    DrawParams   drawParam{};
    DrawCallType drawCall;
    DrawQueryType drawQueryCall;
    float        zoom = 1;
    bool        dirty = 1;
};
} // namespace Draw
} // namespace Voxol::Render
#endif