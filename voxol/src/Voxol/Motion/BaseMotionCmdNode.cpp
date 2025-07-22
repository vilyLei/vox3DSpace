#include "BaseMotionCmdNode.h"
#include <random>
namespace Voxol::Motion
{
namespace Detail
{

std::random_device                    rd;
std::mt19937                          gen(rd());
std::uniform_real_distribution<float> distribute(0.0f, 1.0f);
float                                 makeRandomFloat()
{
    return distribute(gen);
}
void RectDrawCmdDesc::updateToBuffer(uint8_t* buffer)
{
    descSize = static_cast<uint32_t>(sizeof(RectDrawCmdDesc)) / 4;
    //printf("descSize: %d\n", descSize);
    std::memcpy(buffer, this, sizeof(RectDrawCmdDesc));
}


void DrawCmdTestNode::setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
}
void DrawCmdTestNode::init(uint32_t index)
{
    vx = makeRandomFloat() * 2;
    vy = makeRandomFloat() * 2;
}
void DrawCmdTestNode::update(uint32_t index, float boundaryWidth, float boundaryHeight)
{

    auto tx = x + vx;
    auto ty = y + vy;
    auto minX = 0;
    auto minY = 0;
    auto maxX = boundaryWidth;
    auto maxY = boundaryHeight;
    if (vx < 0 && tx < minX)
    {
        vx *= -1;
    }
    else if (vx > 0 && (tx + width) > maxX)
    {
        vx *= -1;
    }

    if (vy < 0 && ty < minY)
    {
        vy *= -1;
    }
    else if (vy > 0 && (ty + height) > maxY)
    {
        vy *= -1;
    }

    x += vx;
    y += vy;
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    mat.setTo(x, y, width, height, rotation);
}
} // namespace Detail
} // namespace Voxol::Motion