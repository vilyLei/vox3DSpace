
#ifndef VOXOL_MAT3_H
#define VOXOL_MAT3_H

#include <array>
#include <initializer_list>
#include <cmath>
#ifdef __EMSCRIPTEN__
// #define APPLY_WASM_SIMD 1
#    include <wasm_simd128.h>
#endif

namespace Voxol::Math
{

/// 2D矩阵类声明，支持SIMD优化、列主序、WebGL/GLES友好
class Mat33
{
public:
    std::array<float, 9> data;

    Mat33();
    Mat33(std::initializer_list<float> list);
    Mat33(float tx, float ty, float sx = 1.0f, float sy = 1.0f, float radians = 0.0f);

    void identity();

    static Mat33 translate(float tx, float ty);
    static Mat33 scale(float sx = 1.0f, float sy = 1.0f);
    static Mat33 rotate(float radians = 0.0f);

    void ortho(float width, float height);

    void  transpose();
    void  prepend(const Mat33& rhs);
    void  append(const Mat33& lhs);
    Mat33 operator*(const Mat33& rhs) const;

#ifdef __EMSCRIPTEN__
    Mat33 multiplySimd(const Mat33& rhs) const;
#endif

    const float* ptr() const;
};
} // namespace Voxol::Math

#endif // VOXOL_MAT3_H