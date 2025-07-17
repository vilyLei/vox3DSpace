#include <algorithm>
#include <cstdio>
#include "Mat33.h"

namespace Voxol::Math
{
Mat33::Mat33()
{
    identity();
}

Mat33::Mat33(std::initializer_list<float> list)
{
    int i = 0;
    for (float v : list)
    {
        if (i < 9) data[i++] = v;
    }
}

Mat33::Mat33(float tx, float ty, float sx, float sy, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);

    data[0] = c * sx;
    data[1] = s * sx;
    data[2] = 0.0f;

    data[3] = -s * sy;
    data[4] = c * sy;
    data[5] = 0.0f;

    data[6] = tx;
    data[7] = ty;
    data[8] = 1.0f;
}

void Mat33::identity()
{
    data = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1};
}

Mat33 Mat33::ortho(float width, float height)
{
    return {
        2.0f / width, 0.0f, 0.0f,
        0.0f, -2.0f / height, 0.0f,
        -1.0f, 1.0f, 1.0f};
}

Mat33 Mat33::translate(float tx, float ty)
{
    return {
        1, 0, 0,
        0, 1, 0,
        tx, ty, 1};
}

Mat33 Mat33::scale(float sx, float sy)
{
    return {
        sx, 0, 0,
        0, sy, 0,
        0, 0, 1};
}

Mat33 Mat33::rotate(float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);
    return {
        c, s, 0,
        -s, c, 0,
        0, 0, 1};
}
void Mat33::transpose()
{
    std::swap(data[1], data[3]);
    std::swap(data[2], data[6]);
    std::swap(data[5], data[7]);
}
// 列主序矩阵的前乘(左乘), 性能更好
void Mat33::prepend(const Mat33& lhs)
{
    // 下面用宏处理，是为了在不同的环境下平衡分支预测和cache line命中的效率

    auto       sfs = data.data();
    const auto lfs = lhs.data.data();

#ifdef __EMSCRIPTEN__
    float          row[3];
    constexpr auto rowSize = sizeof(row);
    for (int i = 0; i < 3; ++i)
    {
        std::memcpy(row, sfs + i * 3, rowSize);
        sfs[i * 3 + 0] = row[0] * lfs[0] + row[1] * lfs[3] + row[2] * lfs[6];
        sfs[i * 3 + 1] = row[0] * lfs[1] + row[1] * lfs[4] + row[2] * lfs[7];
        sfs[i * 3 + 2] = row[0] * lfs[2] + row[1] * lfs[5] + row[2] * lfs[8];
    }
#else
    for (int i = 0; i < 3; ++i)
    {
        float rc0      = sfs[i * 3 + 0];
        float rc1      = sfs[i * 3 + 1];
        float rc2      = sfs[i * 3 + 2];
        sfs[i * 3 + 0] = rc0 * lfs[0] + rc1 * lfs[3] + rc2 * lfs[6];
        sfs[i * 3 + 1] = rc0 * lfs[1] + rc1 * lfs[4] + rc2 * lfs[7];
        sfs[i * 3 + 2] = rc0 * lfs[2] + rc1 * lfs[5] + rc2 * lfs[8];
    }
#endif
}
// 列主序矩阵的后乘(右乘), 性能略弱
void Mat33::append(const Mat33& rhs)
{
    float*       sfs = data.data();
    const float* rfs = rhs.data.data();
    float        result[9];

    for (int i = 0; i < 3; ++i)
    {
        float rc0 = rfs[i * 3 + 0];
        float rc1 = rfs[i * 3 + 1];
        float rc2 = rfs[i * 3 + 2];

        result[i * 3 + 0] = sfs[0] * rc0 + sfs[3] * rc1 + sfs[6] * rc2;
        result[i * 3 + 1] = sfs[1] * rc0 + sfs[4] * rc1 + sfs[7] * rc2;
        result[i * 3 + 2] = sfs[2] * rc0 + sfs[5] * rc1 + sfs[8] * rc2;
    }

    std::memcpy(sfs, result, sizeof(result));
}
Mat33 Mat33::operator*(const Mat33& rhs) const
{
#ifdef APPLY_WASM_SIMD
    return multiplySimd(rhs);
#else
    Mat33 result;
    for (int col = 0; col < 3; ++col)
    {
        for (int row = 0; row < 3; ++row)
        {
            result.data[col * 3 + row] =
                data[0 * 3 + row] * rhs.data[col * 3 + 0] +
                data[1 * 3 + row] * rhs.data[col * 3 + 1] +
                data[2 * 3 + row] * rhs.data[col * 3 + 2];
        }
    }
    return result;
#endif
}

#ifdef __EMSCRIPTEN__
Mat33 Mat33::multiplySimd(const Mat33& rhs) const
{

    printf("Mat33::multiplySimd() ...\n");

    Mat33 out;
    for (int col = 0; col < 3; ++col)
    {
        float b0 = rhs.data[col * 3 + 0];
        float b1 = rhs.data[col * 3 + 1];
        float b2 = rhs.data[col * 3 + 2];

        v128_t a0 = wasm_v128_load(&data[0]);
        v128_t a1 = wasm_v128_load(&data[3]);
        v128_t a2 = wasm_v128_load(&data[6]);

        v128_t c0 = wasm_f32x4_mul(a0, wasm_f32x4_splat(b0));
        v128_t c1 = wasm_f32x4_mul(a1, wasm_f32x4_splat(b1));
        v128_t c2 = wasm_f32x4_mul(a2, wasm_f32x4_splat(b2));

        v128_t sum = wasm_f32x4_add(wasm_f32x4_add(c0, c1), c2);
        wasm_v128_store(&out.data[col * 3], sum);
    }
    return out;
}
#endif

const float* Mat33::ptr() const
{
    return data.data();
}
} // namespace Voxol::Math