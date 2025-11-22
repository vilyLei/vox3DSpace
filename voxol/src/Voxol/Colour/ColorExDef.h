#ifndef VOXOL_COLOUR_COLOR_EX_DEF_H
#define VOXOL_COLOUR_COLOR_EX_DEF_H

#include "ColorDef.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <cassert>

#include <iostream>

namespace Voxol::Colour
{
namespace Component
{
// C++20 header-only minimal ColorEx implementation

// ---------- ColorSpace descriptor ----------
enum class ColorSpaceKind
{
    sRGB,
    DisplayP3,
    AdobeRGB,
    // extend: Rec2020, ACEScg, XYZ, Lab, DeviceICC
};

struct ColorSpaceDesc
{
    ColorSpaceKind kind;
    // primaries->XYZ matrix (3x3) that converts linear RGB (in this space) to CIE XYZ (D65)
    // Row-major: M[row*3 + col]
    std::array<float, 9> linearRGB_to_XYZ;
    std::array<float, 9> XYZ_to_linearRGB;
    // transfer functions (gamma) - we implement sRGB standard funcs; others may be simple gamma
    // For advanced profiles, use ICC provider
    std::string name;
};

// ---------- Precomputed matrices (linear RGB <-> XYZ), D65 ---------
// Values from standard definitions (use widely-known matrices).
// Note: these are floats, good for most rendering tasks. For final print or color-critical work,
// replace with ICC transform via LittleCMS or similar.
//
// sRGB (D65)
// linear RGB -> XYZ
inline constexpr std::array<float, 9> sRGB_lin_to_XYZ = {
    0.412390799f, 0.357584342f, 0.180480788f,
    0.212639005f, 0.715168678f, 0.072192317f,
    0.019330820f, 0.119194779f, 0.950532153f};
// XYZ -> linear RGB (inverse)
inline constexpr std::array<float, 9> sRGB_XYZ_to_lin = {
    3.240969941f, -1.537383177f, -0.498610760f,
    -0.969243636f, 1.875967501f, 0.041555057f,
    0.055630079f, -0.203976958f, 1.056971514f};

// Display P3 (D65) - standard matrix (approx)
inline constexpr std::array<float, 9> P3_lin_to_XYZ = {
    0.486570948f, 0.265667693f, 0.198217285f,
    0.228974564f, 0.691738521f, 0.079286914f,
    0.000000000f, 0.045113381f, 1.043944368f};
inline constexpr std::array<float, 9> P3_XYZ_to_lin = {
    2.493496911f, -0.931383617f, -0.402710784f,
    -0.829488969f, 1.762664060f, 0.023624685f,
    0.035845830f, -0.076172389f, 0.956884524f};

// Adobe RGB (1998) D65
inline constexpr std::array<float, 9> AdobeRGB_lin_to_XYZ = {
    0.5767309f, 0.1855540f, 0.1881852f,
    0.2973769f, 0.6273491f, 0.0752741f,
    0.0270343f, 0.0706872f, 0.9911085f};
inline constexpr std::array<float, 9> AdobeRGB_XYZ_to_lin = {
    2.0413690f, -0.5649464f, -0.3446944f,
    -0.9692660f, 1.8760108f, 0.0415560f,
    0.0134474f, -0.1183897f, 1.0154096f};

// ---------- Helper: matrix multiply 3x3 * vec3 ----------
inline std::array<float, 3> mul3x3_vec3(const std::array<float, 9>& M, const std::array<float, 3>& v)
{
    return {
        M[0] * v[0] + M[1] * v[1] + M[2] * v[2],
        M[3] * v[0] + M[4] * v[1] + M[5] * v[2],
        M[6] * v[0] + M[7] * v[1] + M[8] * v[2]};
}

// ---------- Transfer (gamma) funcs ----------
// sRGB standard transfer - note: careful with branching; we follow spec
inline float srgb_compand_inverse(float u) noexcept
{ // sRGB -> linear
    // u in [0,1]
    if (u <= 0.04045f) return u / 12.92f;
    return std::pow((u + 0.055f) / 1.055f, 2.4f);
}
inline float srgb_compand_forward(float u) noexcept
{ // linear -> sRGB
    if (u <= 0.0031308f) return 12.92f * u;
    return 1.055f * std::pow(u, 1.0f / 2.4f) - 0.055f;
}

// simple gamma (power) for other spaces
inline float gamma_inverse(float u, float gamma) noexcept
{
    return std::pow(u, gamma);
}
inline float gamma_forward(float u, float gamma) noexcept
{
    return std::pow(u, 1.0f / gamma);
}

// ---------- XYZ <-> Lab (D65) ----------
// D65 reference white (Xn, Yn, Zn) for observer 2бу (illuminant D65)
inline constexpr std::array<float, 3> D65_XYZ = {0.95047f, 1.0f, 1.08883f};

inline float lab_f(float t) noexcept
{
    const float delta = 6.0f / 29.0f;
    if (t > delta * delta * delta) return std::cbrt(t);
    return t / (3 * delta * delta) + 4.0f / 29.0f;
}

inline float lab_f_inv(float ft) noexcept
{
    const float delta = 6.0f / 29.0f;
    if (ft > delta) return ft * ft * ft;
    return 3 * delta * delta * (ft - 4.0f / 29.0f);
}

struct Lab
{
    float L;
    float a;
    float b;
};

inline Lab XYZ_to_Lab(const std::array<float, 3>& xyz) noexcept
{
    float xr = xyz[0] / D65_XYZ[0];
    float yr = xyz[1] / D65_XYZ[1];
    float zr = xyz[2] / D65_XYZ[2];

    float fx = lab_f(xr);
    float fy = lab_f(yr);
    float fz = lab_f(zr);

    Lab out;
    out.L = 116.0f * fy - 16.0f;
    out.a = 500.0f * (fx - fy);
    out.b = 200.0f * (fy - fz);
    return out;
}

inline std::array<float, 3> Lab_to_XYZ(const Lab& lab) noexcept
{
    float fy = (lab.L + 16.0f) / 116.0f;
    float fx = fy + lab.a / 500.0f;
    float fz = fy - lab.b / 200.0f;

    float xr = lab_f_inv(fx);
    float yr = lab_f_inv(fy);
    float zr = lab_f_inv(fz);

    return {xr * D65_XYZ[0], yr * D65_XYZ[1], zr * D65_XYZ[2]};
}

// ---------- ColorSpace registry / descriptor provider ----------
class ColorSpaceRegistry
{
public:
    static ColorSpaceRegistry& instance()
    {
        static ColorSpaceRegistry reg;
        return reg;
    }

    ColorSpaceDesc get(ColorSpaceKind k)
    {
        // thread-safe read (we only fill once in ctor)
        return map_.at(k);
    }

private:
    ColorSpaceRegistry()
    {
        map_.emplace(ColorSpaceKind::sRGB,
                     ColorSpaceDesc{ColorSpaceKind::sRGB, sRGB_lin_to_XYZ, sRGB_XYZ_to_lin, "sRGB (D65)"});
        map_.emplace(ColorSpaceKind::DisplayP3,
                     ColorSpaceDesc{ColorSpaceKind::DisplayP3, P3_lin_to_XYZ, P3_XYZ_to_lin, "Display-P3 (D65)"});
        map_.emplace(ColorSpaceKind::AdobeRGB,
                     ColorSpaceDesc{ColorSpaceKind::AdobeRGB, AdobeRGB_lin_to_XYZ, AdobeRGB_XYZ_to_lin, "AdobeRGB (1998)"});
    }
    std::unordered_map<ColorSpaceKind, ColorSpaceDesc> map_;
};

// ---------- ColorEx: linear color + color-space tag ----------
struct ColorEx
{
    // linear RGB [0..+inf) in floats for this color's color space.
    // Using float32 for performance; for color-critical apps use double internally.
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f; // alpha (0..1)

    ColorSpaceKind space = ColorSpaceKind::sRGB;

    constexpr ColorEx() noexcept = default;
    constexpr ColorEx(float rr, float gg, float bb, float aa = 1.0f, ColorSpaceKind s = ColorSpaceKind::sRGB) noexcept :
        r(rr), g(gg), b(bb), a(aa), space(s) {}

    static ColorEx fromColor(const Color& c, ColorSpaceKind srcSpace = ColorSpaceKind::sRGB) noexcept
    {
        // decode 8-bit into [0,1], then linearize using source space transfer
        float rf = static_cast<float>(c.r()) / 255.0f;
        float gf = static_cast<float>(c.g()) / 255.0f;
        float bf = static_cast<float>(c.b()) / 255.0f;
        float af = static_cast<float>(c.a()) / 255.0f;

        ColorEx out;
        out.space = srcSpace;

        if (srcSpace == ColorSpaceKind::sRGB)
        {
            out.r = srgb_compand_inverse(rf);
            out.g = srgb_compand_inverse(gf);
            out.b = srgb_compand_inverse(bf);
            out.a = af;
            return out;
        }
        // other spaces: treat as simple gamma 2.2 by default for now
        float gamma = 2.2f;
        if (srcSpace == ColorSpaceKind::AdobeRGB) gamma = 2.19921875f; // AdobeRGB approx
        // DisplayP3 uses sRGB EOTF (same transfer as sRGB), so use srgb inverse
        if (srcSpace == ColorSpaceKind::DisplayP3)
        {
            out.r = srgb_compand_inverse(rf);
            out.g = srgb_compand_inverse(gf);
            out.b = srgb_compand_inverse(bf);
            out.a = af;
            return out;
        }
        out.r = gamma_inverse(rf, gamma);
        out.g = gamma_inverse(gf, gamma);
        out.b = gamma_inverse(bf, gamma);
        out.a = af;
        return out;
    }

    Color toColor(ColorSpaceKind dstSpace = ColorSpaceKind::sRGB) const noexcept
    {
        // convert linear RGB from this->space to dstSpace, then compand and quantize to 8-bit
        // Steps: linear(this.space) -> XYZ -> linear(dst.space) -> compand(dst) -> 8bit
        // 1) linear(this.space) -> XYZ
        const ColorSpaceDesc srcDesc = ColorSpaceRegistry::instance().get(space);
        std::array<float, 3> rgb_lin = {r, g, b};
        std::array<float, 3> xyz     = mul3x3_vec3(srcDesc.linearRGB_to_XYZ, rgb_lin);

        // 2) XYZ -> linear(dst)
        const ColorSpaceDesc dstDesc     = ColorSpaceRegistry::instance().get(dstSpace);
        std::array<float, 3> rgb_lin_dst = mul3x3_vec3(dstDesc.XYZ_to_linearRGB, xyz);

        // 3) compand to dst encoding
        float rf, gf, bf;
        if (dstSpace == ColorSpaceKind::sRGB)
        {
            rf = srgb_compand_forward(rgb_lin_dst[0]);
            gf = srgb_compand_forward(rgb_lin_dst[1]);
            bf = srgb_compand_forward(rgb_lin_dst[2]);
        }
        else if (dstSpace == ColorSpaceKind::DisplayP3)
        {
            // DisplayP3 uses sRGB-like transfer
            rf = srgb_compand_forward(rgb_lin_dst[0]);
            gf = srgb_compand_forward(rgb_lin_dst[1]);
            bf = srgb_compand_forward(rgb_lin_dst[2]);
        }
        else if (dstSpace == ColorSpaceKind::AdobeRGB)
        {
            // AdobeRGB uses gamma ~2.199
            rf = gamma_forward(rgb_lin_dst[0], 2.19921875f);
            gf = gamma_forward(rgb_lin_dst[1], 2.19921875f);
            bf = gamma_forward(rgb_lin_dst[2], 2.19921875f);
        }
        else
        {
            // fallback: srgb compand
            rf = srgb_compand_forward(rgb_lin_dst[0]);
            gf = srgb_compand_forward(rgb_lin_dst[1]);
            bf = srgb_compand_forward(rgb_lin_dst[2]);
        }

        // 4) clamp and convert to 8-bit
        auto clamp01 = [](float v) noexcept {
            if (v <= 0.0f) return 0.0f;
            if (v >= 1.0f) return 1.0f;
            return v;
        };
        uint8_t r8 = static_cast<uint8_t>(std::lroundf(clamp01(rf) * 255.0f));
        uint8_t g8 = static_cast<uint8_t>(std::lroundf(clamp01(gf) * 255.0f));
        uint8_t b8 = static_cast<uint8_t>(std::lroundf(clamp01(bf) * 255.0f));
        uint8_t a8 = static_cast<uint8_t>(std::lroundf(clamp01(a) * 255.0f));
        return Color(r8, g8, b8, a8);
    }

    // Convert to XYZ
    std::array<float, 3> toXYZ() const noexcept
    {
        const ColorSpaceDesc desc = ColorSpaceRegistry::instance().get(space);
        return mul3x3_vec3(desc.linearRGB_to_XYZ, {r, g, b});
    }

    // Convert to Lab
    Lab toLab() const noexcept
    {
        return XYZ_to_Lab(toXYZ());
    }

    // Gamut clipping in-place for the current space
    void gamutClip() noexcept
    {
        if (r < 0.0f) r = 0.0f;
        if (g < 0.0f) g = 0.0f;
        if (b < 0.0f) b = 0.0f;
        // allow >1 for HDR pipelines
    }
};

// ---------- Utility: Delta E (CIE76 simple) ----------
inline float deltaE76(const Lab& a, const Lab& b) noexcept
{
    float dL = a.L - b.L;
    float da = a.a - b.a;
    float db = a.b - b.b;
    return std::sqrt(dL * dL + da * da + db * db);
}

// ---------- API hooks for ICC provider (placeholder) ----------
// For color-critical workflows, replace pipeline above with ICC transforms.
// Provide an interface that integrates with LittleCMS (or other).
class IColorProfileProvider
{
public:
    virtual ~IColorProfileProvider() = default;
    // convert linear rgb in srcDesc.space to linear rgb in dstDesc.space (in place)
    virtual bool transformLinearRGB(ColorSpaceKind src, const std::array<float, 3>& srcLinear, ColorSpaceKind dst, std::array<float, 3>& outLinear) = 0;
};

// Registry for optional profile provider
class ProfileProviderRegistry
{
public:
    static ProfileProviderRegistry& instance()
    {
        static ProfileProviderRegistry r;
        return r;
    }
    void registerProvider(IColorProfileProvider* p)
    {
        std::lock_guard lk(mutex_);
        provider_ = p;
    }
    IColorProfileProvider* provider()
    {
        std::lock_guard lk(mutex_);
        return provider_;
    }

private:
    std::mutex             mutex_;
    IColorProfileProvider* provider_ = nullptr;
};

// If a provider is registered, ColorEx::toColor / fromColor could be extended
// to use that provider for higher-fidelity transforms. For simplicity this header
// defaults to matrix transforms, but hooks exist for provider usage.

// ---------- Batch conversion helper (useful for performance) ----------
inline void convertColorsBatchToSpace(const std::vector<ColorEx>& src, std::vector<ColorEx>& out, ColorSpaceKind dstSpace)
{
    out.resize(src.size());
    for (size_t i = 0; i < src.size(); ++i)
    {
        // convert each ColorEx to dst linear RGB
        ColorEx c = src[i];
        // if same space, copy
        if (c.space == dstSpace)
        {
            out[i] = c;
            continue;
        }
        // linear(this.space) -> XYZ -> linear(dst.space)
        const ColorSpaceDesc srcDesc = ColorSpaceRegistry::instance().get(c.space);
        std::array<float, 3> xyz     = mul3x3_vec3(srcDesc.linearRGB_to_XYZ, {c.r, c.g, c.b});
        const ColorSpaceDesc dstDesc = ColorSpaceRegistry::instance().get(dstSpace);
        std::array<float, 3> linDst  = mul3x3_vec3(dstDesc.XYZ_to_linearRGB, xyz);
        out[i]                       = ColorEx(linDst[0], linDst[1], linDst[2], c.a, dstSpace);
    }
}
int testColorEx()
{
    // Given your base Color (ARGB 8-bit)
    Color c8(0xFFFF0000u); // opaque red (A=255,R=255,G=0,B=0) ; or Color(255,0,0)

    // Convert to ColorEx (linear) assuming source is sRGB
    ColorEx ex = ColorEx::fromColor(c8, ColorSpaceKind::sRGB);

    // Convert to DisplayP3 and get 8-bit back
    Color p3 = ex.toColor(ColorSpaceKind::DisplayP3);

    std::cout << "P3 ARGB: 0x" << std::hex << p3.argb() << std::dec << "\n";

    // Convert to Lab for comparison
    Lab lab = ex.toLab();
    std::cout << "Lab: L=" << lab.L << " a=" << lab.a << " b=" << lab.b << "\n";

    // Create ColorEx in AdobeRGB linear, then convert to sRGB color
    ColorEx adobe_lin = ColorEx::fromColor(Color(0, 128, 255), ColorSpaceKind::AdobeRGB);
    Color   sRgbOut   = adobe_lin.toColor(ColorSpaceKind::sRGB);
    std::cout << "sRGB ARGB: 0x" << std::hex << sRgbOut.argb() << std::dec << "\n";

    return 0;
}
} // namespace Component
} // namespace Voxol::Colour
#endif