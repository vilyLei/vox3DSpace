#ifndef VOXOL_BASE_MOTION_CMD_NODE_H
#define VOXOL_BASE_MOTION_CMD_NODE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

namespace Voxol::Motion
{

using namespace Voxol::Math;

namespace Detail
{
struct RectDrawCmdDesc
{
    uint32_t rcmd     = 0x33;
    uint32_t descSize = 0x32;
    uint32_t color    = 0xff00aa00;
    Mat33    transform{};

    void updateToBuffer(uint8_t* buffer);
};

struct DrawCmdTestNode
{
    uint32_t rcmd   = 0x33;
    uint32_t color  = 0xff00aa00;
    // velocity
    float    vx      = 0;
    float    vy      = 0;
    float    x      = 0;
    float    y      = 0;
    float    scaleX = 1;
    float    scaleY = 1;
    /// @brief radian value
    float rotation = 0;

    bool dirty = true;

    void setRGB(uint8_t red, uint8_t green, uint8_t blue);
    void init(uint32_t index);
    void update(uint32_t index, float width, float height);
    void updateToMat33(Mat33& mat);
};
} // namespace Detail
class BaseMotionCmdNode
{
public:
    BaseMotionCmdNode()  = default;
    ~BaseMotionCmdNode() = default;

public:
};

} // namespace Voxol::Motion
#endif