
#ifndef VOXOL_MATH_VEC2_H
#define VOXOL_MATH_VEC2_H

#include <cmath>

namespace Voxol::Math
{

//struct Vec2
//{
//public:
//    float x{0};
//    float y{0};
//
//public:
//    Vec2 operator+(const Vec2& other) const;
//    Vec2 operator-(const Vec2& other) const;
//    Vec2 operator*(float s) const;
//    bool isEqual(const Vec2& pos) const;
//};

struct Vec2
{
    union
    {
        struct
        {
            float x, y;
        };
        struct
        {
            float width, height;
        };
        float data[2];
    };
    constexpr Vec2() noexcept :
        data{} {}
    constexpr Vec2(float x_, float y_) :
        x(x_), y(y_) {}

    bool operator==(const Vec2& v) const noexcept { return x == v.x && y == v.y; }
    bool operator!=(const Vec2& v) const noexcept { return !(*this == v); }

    static float Dot(const Vec2& a, const Vec2& b) noexcept { return a.x * b.x + a.y * b.y; }
    static float Cross(const Vec2& a, const Vec2& b) noexcept { return a.x * b.y - a.y * b.x; }

    static Vec2 Normalize(const Vec2& v) noexcept
    {
        float len = v.length();
        return (len > 1e-6f) ? v * (1.0f / len) : Vec2{0, 0};
    }

    constexpr Vec2 operator-() const noexcept { return {-x, -y}; }
    constexpr Vec2 operator+(const Vec2& v) const noexcept { return {x + v.x, y + v.y}; }
    constexpr Vec2 operator-(const Vec2& v) const noexcept { return {x - v.x, y - v.y}; }
    constexpr Vec2 operator*(const Vec2& v) const noexcept { return {x * v.x, y * v.y}; }

    friend constexpr Vec2 operator*(const Vec2& v, float s) noexcept { return {v.x * s, v.y * s}; }
    friend constexpr Vec2 operator*(float s, const Vec2& v) noexcept { return {v.x * s, v.y * s}; }
    friend constexpr Vec2 operator/(const Vec2& v, float s) noexcept { return {v.x / s, v.y / s}; }

    void operator+=(const Vec2& v) noexcept { *this = *this + v; }
    void operator-=(const Vec2& v) noexcept { *this = *this - v; }
    void operator*=(float s) noexcept { *this = *this * s; }
    void operator/=(float s) noexcept { *this = *this / s; }
    void operator*=(const Vec2& v) noexcept { *this = *this * v; } // element-wise multiply

    float lengthSquared() const noexcept { return Dot(*this, *this); }
    float length() const noexcept { return std::sqrt(lengthSquared()); }

    float dot(const Vec2& v) const noexcept { return Dot(*this, v); }
    float cross(const Vec2& v) const noexcept { return Cross(*this, v); }
    Vec2  normalize() const noexcept { return Normalize(*this); }

    const float* ptr() const noexcept { return &x; }
    float*       ptr() noexcept { return &x; }

    inline bool isEqualWithPrecise(const Vec2& pos) const noexcept { return *this == pos; }
    bool        isEqual(const Vec2& pos) const;
};

} // namespace Voxol::Math

#endif // VOXOL_MATH_VEC2_H