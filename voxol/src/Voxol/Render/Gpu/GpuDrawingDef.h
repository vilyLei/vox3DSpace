#ifndef VOXOL_RENDER_GPU_DRAWING_DEF_H
#define VOXOL_RENDER_GPU_DRAWING_DEF_H

#include "../Base/BaseDefine.h"
#include <format>
#include <string>

namespace Voxol::Render::Gpu
{

struct GPUDrawingState
{
    uint32_t value;
    constexpr GPUDrawingState() noexcept :
        value(0xFF000001) {}
    constexpr GPUDrawingState(uint8_t blendMode, uint8_t cullFace, uint8_t depthTest, uint8_t colorMask = 255) noexcept :
        value((static_cast<uint32_t>(colorMask) << 24) |
              (static_cast<uint32_t>(depthTest) << 16) |
              (static_cast<uint32_t>(cullFace) << 8) |
              static_cast<uint32_t>(blendMode)) {}

    constexpr GPUDrawingState(uint32_t state_value) :
        value(state_value) {}

    constexpr uint8_t colorMask() const noexcept { return (value >> 24) & 0xFF; }
    constexpr uint8_t depthTest() const noexcept { return (value >> 16) & 0xFF; }
    constexpr uint8_t cullFace() const noexcept { return (value >> 8) & 0xFF; }
    constexpr uint8_t blendMode() const noexcept { return value & 0xFF; }

    constexpr void reset() noexcept
    {
        value = 0xff000001;
    }

    constexpr void colorMask(uint8_t v) noexcept
    {
        value = (value & 0x00FFFFFF) | (static_cast<uint32_t>(v) << 24);
    }
    constexpr void depthTest(uint8_t v) noexcept
    {
        value = (value & 0xFF00FFFF) | (static_cast<uint32_t>(v) << 16);
    }
    constexpr void cullFace(uint8_t v) noexcept
    {
        value = (value & 0xFFFF00FF) | (static_cast<uint32_t>(v) << 8);
    }
    constexpr void blendMode(uint8_t v) noexcept
    {
        value = (value & 0xFFFFFF00) | static_cast<uint32_t>(v);
    }

    constexpr GPUDrawingState withColorMask(uint8_t v) noexcept
    {
        return GPUDrawingState((value & 0x00FFFFFF) | (static_cast<uint32_t>(v) << 24));
    }
    constexpr GPUDrawingState withDepthTest(uint8_t v) const noexcept
    {
        return GPUDrawingState((value & 0xFF00FFFFu) | (uint32_t(v) << 16));
    }
    constexpr GPUDrawingState withCullFace(uint8_t v) noexcept
    {
        return GPUDrawingState((value & 0xFFFF00FF) | (static_cast<uint32_t>(v) << 8));
    }
    constexpr GPUDrawingState withBlendMode(uint8_t v) noexcept
    {
        return GPUDrawingState((value & 0xFFFFFF00) | static_cast<uint32_t>(v));
    }

    std::string toString() const;
};
} // namespace Voxol::Render::Gpu
#endif