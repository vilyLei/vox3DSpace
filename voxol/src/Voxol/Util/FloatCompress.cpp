#include "FloatCompress.h"
#include <cmath>
#include <algorithm>
namespace Voxol::Util::Float
{

half::half() :
    bits(0) {}

half::half(float f) :
    bits(float32ToFloat16(f)) {}

half::operator float() const
{
    return float16ToFloat32(bits);
}

uint16_t encodeFloatToUInt16(float value, float min, float max)
{
    float norm = (value - min) / (max - min);
    return static_cast<uint16_t>(std::clamp(norm, 0.0f, 1.0f) * 65535.0f + 0.5f);
}

float decodeUInt16ToFloat(uint16_t encoded, float min, float max)
{
    float norm = static_cast<float>(encoded) / 65535.0f;
    return norm * (max - min) + min;
}

Uint16x4 compressFloat4ToU16(const Float4& f, float min, float max)
{
    return {
        encodeFloatToUInt16(f.x, min, max),
        encodeFloatToUInt16(f.y, min, max),
        encodeFloatToUInt16(f.z, min, max),
        encodeFloatToUInt16(f.w, min, max),
    };
}

Float4 decompressU16ToFloat4(const Uint16x4& u, float min, float max)
{
    return {
        decodeUInt16ToFloat(u.x, min, max),
        decodeUInt16ToFloat(u.y, min, max),
        decodeUInt16ToFloat(u.z, min, max),
        decodeUInt16ToFloat(u.w, min, max),
    };
}

uint8_t encodeFloat01ToByte(float value)
{
    return static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
}

float decodeByteToFloat01(uint8_t value)
{
    return static_cast<float>(value) / 255.0f;
}

Byte4 compressFloat4ToByte(const Float4& f)
{
    return {
        encodeFloat01ToByte(f.x),
        encodeFloat01ToByte(f.y),
        encodeFloat01ToByte(f.z),
        encodeFloat01ToByte(f.w),
    };
}

Float4 decompressByteToFloat4(const Byte4& b)
{
    return {
        decodeByteToFloat01(b.r),
        decodeByteToFloat01(b.g),
        decodeByteToFloat01(b.b),
        decodeByteToFloat01(b.a),
    };
}

uint16_t float32ToFloat16(float value)
{
    uint32_t f        = *reinterpret_cast<uint32_t*>(&value);
    uint32_t sign     = (f >> 16) & 0x8000;
    int32_t  exponent = ((f >> 23) & 0xFF) - 127 + 15;
    uint32_t mantissa = (f >> 13) & 0x3FF;

    if (exponent <= 0)
    {
        return static_cast<uint16_t>(sign);
    }
    else if (exponent >= 31)
    {
        return static_cast<uint16_t>(sign | 0x7C00); // Inf
    }
    return static_cast<uint16_t>(sign | (exponent << 10) | mantissa);
}

float float16ToFloat32(uint16_t h)
{

    uint32_t sign     = (h & 0x8000) << 16;
    uint32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = (h & 0x3FF) << 13;

    if (exponent == 0)
    {
        if (mantissa == 0)
        {
            uint32_t f = sign;
            return *reinterpret_cast<float*>(&f); // zero
        }
        exponent = 1;
        while ((mantissa & 0x400000) == 0)
        {
            mantissa <<= 1;
            exponent--;
        }
        mantissa &= 0x3FFFFF;
    }
    else if (exponent == 31)
    {
        uint32_t f = sign | 0x7F800000 | mantissa;
        return *reinterpret_cast<float*>(&f); // Inf or NaN
    }

    exponent   = exponent - 15 + 127;
    uint32_t f = sign | (exponent << 23) | mantissa;
    return *reinterpret_cast<float*>(&f);
}

Uint16x4 compressFloat4ToF16(const Float4& f)
{
    return {
        float32ToFloat16(f.x),
        float32ToFloat16(f.y),
        float32ToFloat16(f.z),
        float32ToFloat16(f.w),
    };
}

Float4 decompressF16ToFloat4(const Uint16x4& u)
{
    return {
        float16ToFloat32(u.x),
        float16ToFloat32(u.y),
        float16ToFloat32(u.z),
        float16ToFloat32(u.w),
    };
}
} // namespace Voxol::Util::Float