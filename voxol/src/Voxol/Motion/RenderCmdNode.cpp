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

Vec2 Vec2::operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
Vec2 Vec2::operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
Vec2 Vec2::operator*(float s) const { return {x * s, y * s}; }

bool Rect::intersects(const Rect& other) const
{
    return !(pos.x + width < other.pos.x || pos.x > other.pos.x + other.width ||
             pos.y + height < other.pos.y || pos.y > other.pos.y + other.height);
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
    // descSize = static_cast<uint32_t>(bytesSize) / 4;
    // printf("descSize: %d, bytesSize: %d\n", descSize, bytesSize);
    std::memcpy(buffer, this, bytesSize);
    // printf("BatchElementCmdDesc::updateToBuffer() BBB ...\n");
    return true;
}

bool RectDrawCmdDesc::updateToBuffer(uint8_t* buffer, size_t bufBytesIndex, size_t bufBytesLength)
{
    constexpr auto bytesSize = sizeof(RectDrawCmdDesc);

    if ((bufBytesIndex + bytesSize) > bufBytesLength)
    {
        // printf("RectDrawCmdDesc::updateToBuffer() AAA ...\n");
        return false;
    }
    descSize = static_cast<uint32_t>(bytesSize) / 4;
    // printf("descSize: %d, bytesSize: %d\n", descSize, bytesSize);
    std::memcpy(buffer, this, bytesSize);
    // printf("RectDrawCmdDesc::updateToBuffer() BBB ...\n");
    return true;
}

// void RectDrawCmdDesc::updateToBuffer(uint8_t* buffer)
// {
//     constexpr auto descBytesSize = sizeof(RectDrawCmdDesc);
//     constexpr auto stride = static_cast<uint32_t>(descBytesSize) / 4;
//     descSize = stride;
//     //printf("descSize: %d\n", descSize);
//     std::memcpy(buffer, this, descBytesSize);
// }


void DrawCmdTestNode::init()
{
    auto vx = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    auto vy = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    moveingNode.velocity = {vx, vy};
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    mat.setTo(x, y, scaleX, scaleY, rotation);
}

bool DrawCmdTestNode::contains(float px, float py) const {
    if(px < 0.0f || py < 0.0f)
        return false;
    if(px > 1.0f || py > 1.0f)
        return false;
    return true;
}
void DrawCmdTestNode::update() {
    if(dirty) {
        dirty = true;
        transform.setTo(x, y, scaleX, scaleY, rotation);
    }
}


} // namespace Voxol::Motion