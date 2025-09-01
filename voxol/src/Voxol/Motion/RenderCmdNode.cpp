#include "RenderCmdNode.h"
#include <random>
namespace Voxol::Motion
{
std::random_device                    rd;
std::mt19937                          gen(rd());
std::uniform_real_distribution<float> distribute(0.0f, 1.0f);
float                                 randomFloatValue()
{
    return distribute(gen);
}


namespace RectTarget
{

// Vec2 Vec2::operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
// Vec2 Vec2::operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
// Vec2 Vec2::operator*(float s) const { return {x * s, y * s}; }


void Rect::setXY(float x, float y)
{
    pos.x = x;
    pos.y = y;
}
void Rect::setSize(float w, float h)
{
    width  = w;
    height = h;
}

bool Rect::intersects(const Rect& other) const
{
    return !(pos.x + width < other.pos.x || pos.x > other.pos.x + other.width ||
             pos.y + height < other.pos.y || pos.y > other.pos.y + other.height);
}

void Rect::outset(float dx, float dy)
{
    pos.x -= dx;
    pos.y -= dy;

    width += dx * 2;
    height += dy * 2;
}


MovingRect::MovingRect(Rect r, Vec2 v) :
    rect(r), velocity(v) {}

void MovingRect::update(float dt, const Rect& boundary)
{
    rect.pos = rect.pos + velocity * dt;

    // 边界反弹
    if (rect.pos.x < boundary.pos.x || rect.pos.x + rect.width > boundary.pos.x + boundary.width)
        velocity.x *= -1;

    if (rect.pos.y < boundary.pos.y || rect.pos.y + rect.height > boundary.pos.y + boundary.height)
        velocity.y *= -1;
}

void handleCollision(MovingRect& a, MovingRect& b)
{
    if (a.rect.intersects(b.rect))
    {
        std::swap(a.velocity, b.velocity);
    }
}

} // namespace RectTarget




bool CameraCmdDesc::updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength)
{
    constexpr auto bytesSize = sizeof(CameraCmdDesc);
    if ((bufBytesIndex + bytesSize) > bufBytesLength)
    {
        return false;
    }
    descSize = static_cast<uint32_t>(bytesSize) / 4;
    // printf("CameraCmdDesc::updateToBuffer(), descSize: %d, bytesSize: %lu\n", descSize, bytesSize);
    std::memcpy(buffer, this, bytesSize);
    return true;
}
bool BatchElementCmdDesc::updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength)
{
    constexpr auto bytesSize = sizeof(BatchElementCmdDesc);

    if ((bufBytesIndex + bytesSize) > bufBytesLength)
    {
        // printf("BatchElementCmdDesc::updateToBuffer() AAA ...\n");
        return false;
    }
    descSize = static_cast<uint32_t>(bytesSize) / 4;
    // printf("descSize: %d, bytesSize: %d\n", descSize, bytesSize);
    std::memcpy(buffer, this, bytesSize);
    // printf("BatchElementCmdDesc::updateToBuffer() BBB ...\n");
    return true;
}

bool DrawingCmdDesc::updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength) const
{
    constexpr auto bytesSize = sizeof(DrawingCmdDesc);

    if ((bufBytesIndex + bytesSize) > bufBytesLength)
    {
        // printf("RectDrawCmdDesc::updateToBuffer() AAA ...\n");
        return false;
    }
    // printf("descSize: %d, bytesSize: %d\n", descSize, bytesSize);
    std::memcpy(buffer, this, bytesSize);
    // printf("RectDrawCmdDesc::updateToBuffer() BBB ...\n");
    return true;
}
void DrawingCmdDesc::update()
{
    constexpr auto bytesSize = sizeof(DrawingCmdDesc);
    descSize                 = static_cast<uint32_t>(bytesSize) / 4;
}


void DrawCmdTestNode::setXY(float x, float y)
{
    drcDesc.bounds.setXY(x, y);
}
void DrawCmdTestNode::setSize(float w, float h)
{
    drcDesc.bounds.setSize(w, h);
}

void DrawCmdTestNode::init()
{
    auto vx              = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    auto vy              = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    moveingNode.velocity = {vx, vy};
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    auto& pos = bounds.pos;
    drcDesc.transform.setTo(pos.x, pos.y, bounds.width, bounds.height, rotation);
}

bool DrawCmdTestNode::contains(float px, float py) const
{
    if (px < 0.0f || py < 0.0f)
        return false;
    if (px > 1.0f || py > 1.0f)
        return false;
    return true;
}
void DrawCmdTestNode::update()
{
    if (dirty)
    {
        drcDesc.update();
        auto& pos = bounds.pos;
        drcDesc.transform.setTo(pos.x, pos.y, bounds.width, bounds.height, rotation);
        dirty = true;
    }
}


} // namespace Voxol::Motion