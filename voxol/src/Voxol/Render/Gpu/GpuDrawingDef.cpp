#include "GpuDrawingDef.h"
namespace Voxol::Render::Gpu
{
std::string GPUDrawingState::toString() const
{
    return std::format("GPUDrawingState(colorMask={:02X}, depthTest={:02X},cullFace={:02X},blendMode={:02X}", colorMask(), depthTest(), cullFace(), blendMode());
}
} // namespace Voxol::Render::Gpu