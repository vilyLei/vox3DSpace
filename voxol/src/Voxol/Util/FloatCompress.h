#ifndef VOXOL_FLOT_COMPRESS_H
#define VOXOL_FLOT_COMPRESS_H

#include <cstdint>

namespace Voxol::Util::Float
{

struct Float4
{
    float x, y, z, w;
};

struct Uint16x4
{
    uint16_t x, y, z, w;
};

struct Byte4
{
    uint8_t r, g, b, a;
};

struct half
{
    uint16_t bits;

    half();
    half(float f);
    operator float() const;
};

uint16_t encodeFloatToUInt16(float value, float min, float max);
float    decodeUInt16ToFloat(uint16_t encoded, float min, float max);
Uint16x4 compressFloat4ToU16(const Float4& f, float min, float max);
Float4   decompressU16ToFloat4(const Uint16x4& u, float min, float max);

uint8_t encodeFloat01ToByte(float value);
float   decodeByteToFloat01(uint8_t value);
Byte4   compressFloat4ToByte(const Float4& f);
Float4  decompressByteToFloat4(const Byte4& b);

uint16_t float32ToFloat16(float value);
float    float16ToFloat32(uint16_t h);
Uint16x4 compressFloat4ToF16(const Float4& f);
Float4   decompressF16ToFloat4(const Uint16x4& u);

} // namespace Voxol::Util::Float

#endif