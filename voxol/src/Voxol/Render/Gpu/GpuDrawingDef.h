#ifndef VOXOL_RENDER_GPU_DRAWING_DEF_H
#define VOXOL_RENDER_GPU_DRAWING_DEF_H

#include "../Base/BaseDefine.h"
#include <format>
#include <string>

namespace Voxol::Render::Gpu
{
    /*
    case 21:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ZERO);
                break;
            case 22:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case 23:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case 24:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                break;
            case 25:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_SRC_ALPHA);
                break;
            case 26:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_COLOR, GL_ONE);
                break;
            case 20:
                glDisable(GL_BLEND);
    */
struct DrawingBlendMode
{
    // glBlendFunc(GL_ONE, GL_ZERO);
    static constexpr uint8_t Opaque = 1;
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    static constexpr uint8_t PreMultiAlpha = 2;
    // glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    static constexpr uint8_t Transparent = 3;
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    static constexpr uint8_t Alpha = 4;

    // glDisable(GL_BLEND);
    static constexpr uint8_t None = 20;

    // glBlendFunc(GL_ONE, GL_ONE);
    static constexpr uint8_t Add = 24;

    // glBlendFunc(GL_DST_COLOR, GL_DST_ALPHA);
    static constexpr uint8_t Overlay = 27;
    // glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);
    static constexpr uint8_t Overlay2 = 28;

    static uint8_t fromString(const std::string& name);
};
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