#ifndef VOXOL_RENDER_CMD_NODE_H
#define VOXOL_RENDER_CMD_NODE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"

namespace Voxol::Motion
{

using namespace Voxol::Math;

namespace RectTarget
{

struct MovingRect
{
    VxRect rect{};
    Vec2 velocity{};
    MovingRect() = default;
    MovingRect(VxRect r, Vec2 v);
    void update(float dt, const VxRect& boundary);
};

void handleCollision(MovingRect& a, MovingRect& b);

} // namespace RectTarget

struct CameraCmdDesc
{
    uint32_t rcmd     = 0x30;
    uint32_t descSize = 0x20;
    Mat33    projMat{};
    Mat33    viewMat{};

    bool updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength);
};
struct BatchElementCmdDesc
{
    uint32_t rcmd     = 0x31;
    uint32_t descSize = 0x20;
    uint32_t total = 0;
    bool     updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength);
};
struct DrawingCmdDesc
{
    uint32_t         rcmd     = 0x32;
    uint32_t         descSize = 0x20;
    VxRect bounds{};
    uint32_t         mroid = 0;
    uint32_t         color = 0xff00aa00;
    Mat33            transform{};

    bool updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength) const;
    void update();
};

struct DrawCmdTestNode
{    
    VxRect bounds{};
    /// radian value
    float rotation = 0;
    bool  dirty = true;

    RectTarget::MovingRect moveingNode{};
    DrawingCmdDesc drcDesc{};


    void setXY(float x, float y);
    void setSize(float w, float h);

    void init();
    void updateToMat33(Mat33& mat);
    bool contains(float px, float py) const;
    void update();
};

} // namespace Voxol::Motion
#endif