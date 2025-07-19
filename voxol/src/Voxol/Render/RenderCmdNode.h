#ifndef VOXOL_RENDER_CMD_NODE_H
#define VOXOL_RENDER_CMD_NODE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

namespace Voxol::Render
{
using namespace Voxol::Math;

struct RectDrawCmdDesc
{
    uint32_t rcmd  = 0x33;
    uint32_t descSize   = 0x32;
    uint32_t color = 0xff00aa00;
    Mat33    transform{};

    void updateToBuffer(uint8_t* buffer);
};

struct DrawCmdTestNode
{
    uint32_t rcmd   = 0x33;
    uint32_t color  = 0xff00aa00;
    float    x      = 0;
    float    y      = 0;
    float    scaleX = 1;
    float    scaleY = 1;
    /// @brief radian value
    float rotation = 0;

    bool dirty = true;

    void updateToMat33(Mat33& mat);
};

class RenderCmdNode
{
private:
    /* data */
public:
    RenderCmdNode()  = default;
    ~RenderCmdNode() = default;
};

} // namespace Voxol::Render
#endif