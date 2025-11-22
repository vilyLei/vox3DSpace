#ifndef VOXOL_COLOUR_COLOR_UTILS_H
#define VOXOL_COLOUR_COLOR_UTILS_H

#include "ColorUtils.h"
#include <cstdint>
#include <array>
#include <string>
#include <format>

namespace Voxol::Colour
{
namespace Component
{

struct Color
{
    // default format: argb
    uint32_t value;
    constexpr Color() noexcept :
        value(0xFF000000) {}
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept :
        value((static_cast<uint32_t>(a) << 24) |
              (static_cast<uint32_t>(r) << 16) |
              (static_cast<uint32_t>(g) << 8) |
              static_cast<uint32_t>(b)) {}

    constexpr Color(uint32_t argb_value) :
        value(argb_value) {}

    constexpr uint8_t      a() const noexcept { return (value >> 24) & 0xFF; }
    constexpr uint8_t      r() const noexcept { return (value >> 16) & 0xFF; }
    constexpr uint8_t      g() const noexcept { return (value >> 8) & 0xFF; }
    constexpr uint8_t      b() const noexcept { return value & 0xFF; }

    constexpr uint32_t argb() const noexcept { return value; }
    constexpr uint32_t rgba() const noexcept
    {
        return (uint32_t(r()) << 24) | (uint32_t(g()) << 16) |
            (uint32_t(b()) << 8) | uint32_t(a());
    }
    constexpr uint32_t bgra() const noexcept
    {
        return (uint32_t(b()) << 24) | (uint32_t(g()) << 16) |
            (uint32_t(r()) << 8) | uint32_t(a());
    }
    constexpr uint32_t abgr() const noexcept
    {
        return (uint32_t(a()) << 24) | (uint32_t(b()) << 16) |
            (uint32_t(g()) << 8) | uint32_t(r());
    }

    constexpr void setA(uint8_t a) noexcept
    {
        value = (value & 0x00FFFFFF) | (static_cast<uint32_t>(a) << 24);
    }
    constexpr void setR(uint8_t r) noexcept
    {
        value = (value & 0xFF00FFFF) | (static_cast<uint32_t>(r) << 16);
    }
    constexpr void setG(uint8_t g) noexcept
    {
        value = (value & 0xFFFF00FF) | (static_cast<uint32_t>(g) << 8);
    }
    constexpr void setB(uint8_t b) noexcept
    {
        value = (value & 0xFFFFFF00) | static_cast<uint32_t>(b);
    }

    constexpr Color WithA(uint8_t a) noexcept
    {
        return Color((value & 0x00FFFFFF) | (static_cast<uint32_t>(a) << 24));
    }
    constexpr Color withR(uint8_t nr) const noexcept
    {
        return Color((value & 0xFF00FFFFu) | (uint32_t(nr) << 16));
    }
    constexpr Color withG(uint8_t g) noexcept
    {
        return Color((value & 0xFFFF00FF) | (static_cast<uint32_t>(g) << 8));
    }
    constexpr Color withB(uint8_t b) noexcept
    {
        return Color((value & 0xFFFFFF00) | static_cast<uint32_t>(b));
    }
    constexpr bool isTransparent() const noexcept { return a() < 255; }
    constexpr bool isOpaque() const noexcept { return a() == 255; }

    constexpr std::array<uint8_t, 4> toARGBBytes() const noexcept
    {
        return {a(), r(), g(), b()};
    }
    constexpr std::array<uint8_t, 4> toRGBABytes() const noexcept
    {
        return {r(), g(), b(), a()};
    }
    constexpr std::array<float, 4> toARGBF4() const noexcept
    {
        return {a() / 255.0f, r() / 255.0f, g() / 255.0f, b() / 255.0f};
    }
    constexpr std::array<float, 4> toRGBABF4() const noexcept
    {
        return {r() / 255.0f, g() / 255.0f, b() / 255.0f, a() / 255.0f};
    }

    static constexpr Color makeFromARGB(uint32_t argb)
    {
        return Color(argb);
    }
    static constexpr Color makeFromRGBA(uint32_t rgba)
    {
        return Color(
            (rgba >> 24) & 0xFF, // r
            (rgba >> 16) & 0xFF, // g
            (rgba >> 8) & 0xFF,  // b
            rgba & 0xFF          // a
        );
    }
    std::string toString(std::string_view fmt = "") const
    {
        if (fmt == "hex")
        {
            return std::format("#{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
        }
        else if (fmt == "argb")
        {
            return std::format("ARGB({:02X}{:02X}{:02X}{:02X})", a(), r(), g(), b());
        }
        else if (fmt == "rgba")
        {
            return std::format("RGBA({:02X}{:02X}{:02X}{:02X})", r(), g(), b(), a());
        }
        else if (fmt == "css")
        {
            if (a() == 255)
            {
                return std::format("rgb({}, {}, {})", r(), g(), b());
            }
            else
            {
                return std::format("rgba({}, {}, {}, {:.2f})", r(), g(), b(), a() / 255.0f);
            }
        }
        else
        {
            return std::format("Color({}, {}, {}, {})", r(), g(), b(), a());
        }
    }
    std::string toHexString() const
    {
        return std::format("0x{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
    }
    std::string toCSSString() const
    {
        return std::format("#{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
    }
};
} // namespace Component
} // namespace Voxol::Colour
#endif