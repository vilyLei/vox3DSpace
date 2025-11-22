#ifndef VOXOL_COLOUR_COLOR_UTILS_H
#define VOXOL_COLOUR_COLOR_UTILS_H

#include "../Base/BaseDefine.h"
#include <cstdint>

namespace Voxol::Colour
{
namespace Component
{
struct Color
{
    // default format: argb
    union
    {
        struct
        {
            uint8_t a, r, g, b;
        };
        uint8_t data[4];
        // stored as ARGB (0xAARRGGBB)
        uint32_t value = 0xFF000000;
    };
    constexpr Color() noexcept :
        data{} {}
    constexpr Color(uint8_t r_, uint8_t g_, uint8_t b_, float a_ = 255) :
        r(r_), g(g_), b(b_), a(a_) {}
    uint32_t argb() {
        return value;
    }
    uint32_t rgba()
    {
        return r >> 24 | g << 16 | b << 8 | a;
    }
    uint32_t setRGB(uint8_t r_, uint8_t g_, uint8_t b_)
    {
        r = r_;
        g = g_;
        b = b_;
    }
    uint32_t setAlpha(uint8_t a_)
    {
        a = a_;
    }
    uint32_t setRGBA(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
    {
        r = r_;
        g = g_;
        b = b_;
        a = a_;
    }
};
}
}
#endif