#ifndef VOXOL_MATH_MAT33_H
#define VOXOL_MATH_MAT33_H

#include <array>
#include <initializer_list>
#include <cmath>
#ifdef __EMSCRIPTEN__
// #define APPLY_WASM_SIMD 1
#    include <wasm_simd128.h>
#endif
#include "Vec2.h"

namespace Voxol::Math
{

/// 2D矩阵类声明，支持SIMD优化、列主序、WebGL/GLES友好
class Mat33
{
public:
    union
    {
        float data[9];
        struct
        {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
    };

    constexpr Mat33() noexcept
        :
        data{1, 0, 0,
             0, 1, 0,
             0, 0, 1} {}
    constexpr Mat33(float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) noexcept
        :
        data{a00, a01, a02, a10, a11, a12, a20, a21, a22} {}

    static constexpr Mat33 makeIdentity() noexcept
    {
        return {1, 0, 0,
                0, 1, 0,
                0, 0, 1};
    }
    static constexpr Mat33 makeZero() noexcept
    {
        return {0, 0, 0,
                0, 0, 0,
                0, 0, 0};
    }

    static Mat33 makeWithTransScaleRot(float tx, float ty, float sx = 1, float sy = 1, float rad = 0);
    static Mat33 makeTranslate(float tx, float ty);
    static Mat33 makeScale(float sx = 1.0f, float sy = 1.0f);
    static Mat33 makeRotate(float radians = 0.0f);

    void identity();
    void ortho(float width, float height);

    void setTo(float tx, float ty, float sx = 1.0f, float sy = 1.0f, float rotRadians = 0.0f);
    void setTranslateAndScale(float tx, float ty, float sx = 1.0f, float sy = 1.0f);
    void offsetXY(const Vec2& dv);
    void setXY(const Vec2& pos);
    void setXY(float tx, float ty);
    Vec2 getXY() const;

    void setScaleXY(const Vec2& scaleXY);
    void setScaleXY(float sx, float sy);
    Vec2 getScaleXY() const;

    void  transpose();
    void  prepend(const Mat33& rhs);
    void  append(const Mat33& lhs);
    Mat33 operator*(const Mat33& rhs) const;

#ifdef __EMSCRIPTEN__
    Mat33 multiplySimd(const Mat33& rhs) const;
#endif

    //const float* ptr() const;
    constexpr const float* ptr() const noexcept { return data; }
    constexpr float*       ptr() noexcept { return data; }
    constexpr float&       operator()(size_t row, size_t col) noexcept
    {
        return data[row * 3 + col];
    }
    constexpr const float& operator()(size_t row, size_t col) const noexcept
    {
        return data[row * 3 + col];
    }


    Vec2         mapPoint(const Vec2& point) const;
    Vec2         mapXY(float x, float y) const;

    bool inverseTo(Mat33& lhs) const;
    bool inverse();

    void print() const;
};
namespace Mat33Utils
{
void makeRotationMat33WithPivot(Mat33& transform, Vec2 localPivot, Vec2 fixCV, float scaleX, float scaleY, float rotation);
Mat33 makeRotationMat33WithPivot(Vec2 localPivot, Vec2 fixCV, float scaleX, float scaleY, float rotation);
Mat33 makeRotationMat33WithCenter(Vec2 fixCV, float scaleX, float scaleY, float rotation);
} // namespace Mat33Utils
} // namespace Voxol::Math

#endif // VOXOLMATH_MAT33_H