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
void RectDrawCmdDesc::updateToBuffer(uint8_t* buffer)
{
    descSize = static_cast<uint32_t>(sizeof(RectDrawCmdDesc)) / 4;
    // printf("descSize: :%d\n", descSize);
    std::memcpy(buffer, this, sizeof(RectDrawCmdDesc));
}


void DrawCmdTestNode::init()
{
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    mat.setTo(x, y, scaleX, scaleY, rotation);
}
} // namespace Voxol::Motion