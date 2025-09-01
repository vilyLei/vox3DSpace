
#ifndef VOXOL_MATH_VEC2_H
#define VOXOL_MATH_VEC2_H

#include <cmath>

namespace Voxol::Math
{
struct Vec2
{
public:
    float x{0};
    float y{0};

public:
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float s) const;
};
} // namespace Voxol::Math

#endif // VOXOL_MATH_VEC2_H