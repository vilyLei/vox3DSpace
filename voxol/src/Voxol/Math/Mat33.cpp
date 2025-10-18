#include <algorithm>
#include <cstdio>
#include "Mat33.h"

namespace Voxol::Math
{
//Mat33::Mat33()
//{
//    identity();
//}

//Mat33::Mat33(std::initializer_list<float> list)
//{
//    int i = 0;
//    for (float v : list)
//    {
//        if (i < 9) data[i++] = v;
//    }
//}
//
//Mat33::Mat33(float tx, float ty, float sx, float sy, float radians)
//{
//    setTo(tx, ty, sx, sy, radians);
//}

constexpr Mat33::Mat33() noexcept
    :
    data{1, 0, 0,
         0, 1, 0,
         0, 0, 1} {}

constexpr Mat33::Mat33(float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) noexcept
    :
    data{a00, a01, a02, a10, a11, a12, a20, a21, a22} {}

void Mat33::identity()
{
    float tdata[] = {1, 0, 0,
                        0, 1, 0,
                        0, 0, 1};
    std::copy(std::begin(tdata), std::end(tdata), data);
}


Mat33 Mat33::makeWithTransScaleRot(float tx, float ty, float sx, float sy, float rad) {
    Mat33 mat;
    mat.setTo(tx, ty, sx, sy, rad);
    return mat;
}

Mat33 Mat33::makeTranslate(float tx, float ty)
{
    return Mat33(
        1, 0, 0,
        0, 1, 0,
        tx, ty, 1);
}

Mat33 Mat33::makeScale(float sx, float sy)
{
    return Mat33(
        sx, 0, 0,
        0, sy, 0,
        0, 0, 1);
}

Mat33 Mat33::makeRotate(float rotRadians)
{
    float c = cosf(rotRadians);
    float s = sinf(rotRadians);
    return {
        c, s, 0,
        -s, c, 0,
        0, 0, 1};
}


void Mat33::setTo(float tx, float ty, float sx, float sy, float rotRadians)
{
    float c = 1;
    float s = 0;
    if (std::abs(rotRadians) > 1e-5f)
    {
        c = cosf(rotRadians);
        s = sinf(rotRadians);
    }

    float tdata[9] = {
        c * sx, s * sx, 0,
        -s * sy, c * sy, 0,
        tx, ty, 1};
    std::copy(std::begin(tdata), std::end(tdata), data);
}

void Mat33::setTranslateAndScale(float tx, float ty, float sx, float sy)
{
    data[0] = sx;
    data[4] = sy;
    data[6] = tx;
    data[7] = ty;
}

void Mat33::setXY(const Vec2& pos)
{
    data[6] = pos.x;
    data[7] = pos.y;
}

void Mat33::setXY(float tx, float ty)
{
    data[6] = tx;
    data[7] = ty;
}

Vec2 Mat33::getXY() const
{
    return {data[6], data[7]};
}

void Mat33::setScaleXY(const Vec2& scaleXY)
{
    data[0] = scaleXY.x;
    data[4] = scaleXY.y;
}
void Mat33::setScaleXY(float sx, float sy)
{
    data[0] = sx;
    data[4] = sy;
}
Vec2 Mat33::getScaleXY() const
{
    return {data[0], data[4]};
}
void Mat33::ortho(float width, float height)
{
    float tdata[9] = {
        2.0f / width, 0.0f, 0.0f,
        0.0f, -2.0f / height, 0.0f,
        -1.0f, 1.0f, 1.0f};
    std::copy(std::begin(tdata), std::end(tdata), data);
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

    auto       sfs = data;
    const auto lfs = lhs.data;

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
    float*       sfs = data;
    const float* rfs = rhs.data;
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


Vec2 Mat33::mapPoint(const Vec2& point) const
{
    float px = data[0] * point.x + data[3] * point.y + data[6];
    float py = data[1] * point.x + data[4] * point.y + data[7];
    return {px, py};
}

Vec2 Mat33::mapXY(float x, float y) const
{
    float px = data[0] * x + data[3] * y + data[6];
    float py = data[1] * x + data[4] * y + data[7];
    return {px, py};
}

bool Mat33::inverseTo(Mat33& lhs) const
{
    float det =
        data[0] * (data[4] * data[8] - data[5] * data[7]) -
        data[3] * (data[1] * data[8] - data[2] * data[7]) +
        data[6] * (data[1] * data[5] - data[2] * data[4]);

    if (std::fabs(det) < 1e-8f)
    {
        return false;
    }

    float invDet = 1.0f / det;

    auto& invData = lhs.data;
    invData[0]    = (data[4] * data[8] - data[5] * data[7]) * invDet;
    invData[1]    = -(data[1] * data[8] - data[2] * data[7]) * invDet;
    invData[2]    = (data[1] * data[5] - data[2] * data[4]) * invDet;

    invData[3] = -(data[3] * data[8] - data[5] * data[6]) * invDet;
    invData[4] = (data[0] * data[8] - data[2] * data[6]) * invDet;
    invData[5] = -(data[0] * data[5] - data[2] * data[3]) * invDet;

    invData[6] = (data[3] * data[7] - data[4] * data[6]) * invDet;
    invData[7] = -(data[0] * data[7] - data[1] * data[6]) * invDet;
    invData[8] = (data[0] * data[4] - data[1] * data[3]) * invDet;

    return true;
}

bool Mat33::inverse()
{
    float det =
        data[0] * (data[4] * data[8] - data[5] * data[7]) -
        data[3] * (data[1] * data[8] - data[2] * data[7]) +
        data[6] * (data[1] * data[5] - data[2] * data[4]);

    if (std::fabs(det) < 1e-8f)
    {
        return false;
    }

    float invDet = 1.0f / det;

    auto d0 = (data[4] * data[8] - data[5] * data[7]) * invDet;
    auto d1 = -(data[1] * data[8] - data[2] * data[7]) * invDet;
    auto d2 = (data[1] * data[5] - data[2] * data[4]) * invDet;

    auto d3 = -(data[3] * data[8] - data[5] * data[6]) * invDet;
    auto d4 = (data[0] * data[8] - data[2] * data[6]) * invDet;
    auto d5 = -(data[0] * data[5] - data[2] * data[3]) * invDet;

    auto d6 = (data[3] * data[7] - data[4] * data[6]) * invDet;
    auto d7 = -(data[0] * data[7] - data[1] * data[6]) * invDet;
    auto d8 = (data[0] * data[4] - data[1] * data[3]) * invDet;


    data[0] = d0;
    data[1] = d1;
    data[2] = d2;

    data[3] = d3;
    data[4] = d4;
    data[5] = d5;

    data[6] = d6;
    data[7] = d7;
    data[8] = d8;

    return true;
}


//const float* Mat33::ptr() const
//{
//    return data;
//}
void Mat33::print() const
{
    printf("{\n");
    for (int i = 0; i < 3; ++i)
    {
        printf("%.4f  %.4f  %.4f\n",
               data[i * 3 + 0],
               data[i * 3 + 1],
               data[i * 3 + 2]);
    }
    printf("}\n");
}

namespace Mat33Utils
{

void makeRotationMat33WithPivot(Mat33& transform, Vec2 localPivot, Vec2 fixCV, float scaleX, float scaleY, float rotation)
{

    transform.setTo(scaleX - localPivot.x, scaleY - localPivot.y, scaleX, scaleY, rotation);
    auto&& cv   = transform.mapPoint({localPivot.x / scaleX, localPivot.y / scaleY});
    auto&  data = transform.data;
    data[6] += fixCV.x - cv.x;
    data[7] += fixCV.y - cv.y;
}
Mat33 makeRotationMat33WithPivot(Vec2 localPivot, Vec2 fixCV, float scaleX, float scaleY, float rotation)
{
    Mat33  mat;
    mat.setTo(scaleX - localPivot.x, scaleY - localPivot.y, scaleX, scaleY, rotation);
    auto&& cv   = mat.mapPoint({localPivot.x / scaleX, localPivot.y / scaleY});
    auto&  data = mat.data;
    data[6] += fixCV.x - cv.x;
    data[7] += fixCV.y - cv.y;
    return mat;
}
Mat33 makeRotationMat33WithCenter(Vec2 fixCV, float scaleX, float scaleY, float rotation)
{
    Mat33  mat;
    mat.setTo(scaleX * -0.5f, scaleY * -0.5f, scaleX, scaleY, rotation);
    auto&& tempCV = mat.mapPoint({0.5f, 0.5f});
    auto&  data   = mat.data;
    data[6] += fixCV.x - tempCV.x;
    data[7] += fixCV.y - tempCV.y;
    return mat;
}
} // namespace Mat33Utils
} // namespace Voxol::Math