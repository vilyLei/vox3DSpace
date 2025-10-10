
#ifndef VOXOLMATH_MAT33_H
#define VOXOLMATH_MAT33_H

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
    std::array<float, 9> data;

    Mat33();
    Mat33(std::initializer_list<float> list);
    Mat33(float tx, float ty, float sx = 1.0f, float sy = 1.0f, float rotRadians = 0.0f);

    void identity();

    static Mat33 makeTranslate(float tx, float ty);
    static Mat33 makeScale(float sx = 1.0f, float sy = 1.0f);
    static Mat33 makeRotate(float radians = 0.0f);

    void ortho(float width, float height);

    void setTo(float tx, float ty, float sx = 1.0f, float sy = 1.0f, float rotRadians = 0.0f);
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

    const float* ptr() const;
    Vec2         mapPoint(const Vec2& point) const;
    Vec2         mapXY(float x, float y) const;

    bool inverseTo(Mat33& lhs) const;

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