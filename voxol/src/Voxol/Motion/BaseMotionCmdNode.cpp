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
void DrawCmdTestNode::update(uint32_t index, float width, float height)
{
    x += vx;
    y += vy;
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    mat.setTo(x, y, scaleX, scaleY, rotation);
}
} // namespace Detail
} // namespace Voxol::Motion