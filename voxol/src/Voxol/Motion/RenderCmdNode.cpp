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

MovingRect::MovingRect(VxRect r, Vec2 v) :
    rect(r), velocity(v) {}

void MovingRect::update(float dt, const VxRect& boundary)
{
    rect.min = rect.min + velocity * dt;

    // a simple simulation: rebound from the boundary
    if (rect.fX < boundary.fX || rect.fX + rect.width() > boundary.fX + boundary.width())
        velocity.x *= -1;
    if (rect.fY < boundary.fY || rect.fY + rect.height() > boundary.fY + boundary.height())
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
    drcDesc.bounds.moveTo(x, y);
}
void DrawCmdTestNode::setSize(float w, float h)
{
    drcDesc.bounds.size(w, h);
}

void DrawCmdTestNode::init()
{
    auto vx              = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    auto vy              = (randomFloatValue() * 0.5f - 0.5f) * 2 + randomFloatValue();
    moveingNode.velocity = {vx, vy};
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    drcDesc.transform.setTo(bounds.fX, bounds.fY, bounds.width(), bounds.height(), rotation);
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
        drcDesc.transform.setTo(bounds.fX, bounds.fY, bounds.width(), bounds.height(), rotation);
        dirty = true;
    }
}


} // namespace Voxol::Motion