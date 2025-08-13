#ifndef VOXOL_RENDER_CMD_NODE_H
#define VOXOL_RENDER_CMD_NODE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

namespace Voxol::Motion
{

using namespace Voxol::Math;

namespace RectTarget
{

struct Vec2
{
    float x, y;

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float s) const;
};

struct Rect
{
    Vec2  pos;
    float width;
    float height;

    bool intersects(const Rect& other) const;
};

struct MovingRect
{
    Rect rect;
    Vec2 velocity;
    MovingRect() = default;
    MovingRect(Rect r, Vec2 v);

    void update(float dt, const Rect& boundary);
};

void handleCollision(MovingRect& a, MovingRect& b);

} // namespace RectTarget

struct CameraCmdDesc
{
    uint32_t         rcmd     = 0x30;
    uint32_t         descSize = 0x20;
    Mat33            projMat{};
    Mat33            viewMat{};

    bool updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength);
};
struct BatchElementCmdDesc
{
    uint32_t         rcmd     = 0x31;
    uint32_t         descSize = 0x20;
    bool updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength);
};
struct RectDrawCmdDesc
{
    uint32_t         rcmd     = 0x32;
    uint32_t         descSize = 0x20;
    RectTarget::Rect bounds{};
    uint32_t         color = 0xff00aa00;
    Mat33            transform{};

    bool updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength);
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

    Mat33 transform{};
    bool  dirty = true;

    RectTarget::MovingRect moveingNode{};

    void init();
    void updateToMat33(Mat33& mat);
    bool contains(float px, float py) const;
    void update();
};

class RenderCmdNode
{
private:
    /* data */
public:
    RenderCmdNode()  = default;
    ~RenderCmdNode() = default;
};

} // namespace Voxol::Motion
#endif