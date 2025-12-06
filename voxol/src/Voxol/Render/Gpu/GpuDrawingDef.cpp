#include "GpuDrawingDef.h"
namespace Voxol::Render::Gpu
{

uint8_t DrawingBlendMode::fromString(const std::string& name)
{
    if (name == "alpha")
        return DrawingBlendMode::Alpha;
    if (name == "transparent")
        return DrawingBlendMode::Transparent;
    if (name == "add")
        return DrawingBlendMode::Add;
    if (name == "opaque")
        return DrawingBlendMode::Opaque;
    if (name == "overlay")
        return DrawingBlendMode::Overlay;
    if (name == "overlay2")
        return DrawingBlendMode::Overlay2;

    return DrawingBlendMode::None;
}
std::string GPUDrawingState::toString() const
{
    return std::format("GPUDrawingState(colorMask={:02X}, depthTest={:02X},cullFace={:02X},blendMode={:02X}", colorMask(), depthTest(), cullFace(), blendMode());
}
} // namespace Voxol::Render::Gpu