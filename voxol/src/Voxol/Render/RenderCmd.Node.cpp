#include "RenderCmdNode.h"

namespace Voxol::Render
{

void RectDrawCmdDesc::updateToBuffer(uint8_t* buffer)
{
    descSize = static_cast<uint32_t>(sizeof(RectDrawCmdDesc))/4;
    printf("descSize: :%d\n", descSize);
    std::memcpy(buffer, this, sizeof(RectDrawCmdDesc));
}
void DrawCmdTestNode::updateToMat33(Mat33& mat)
{
    mat.setTo(x, y, scaleX, scaleY, rotation);
}
} // namespace Voxol::Render