
#ifndef VOXOL_MATH_VEC4_H
#define VOXOL_MATH_VEC4_H

#include <cmath>

namespace Voxol::Math
{
struct Vec4
{
    union
    {
        struct
        {
            float x, y, z, w;
        };
        float data[4];
    };
    constexpr Vec4() noexcept :
        data{} {}
    constexpr Vec4(float x_, float y_, float z_, float w_ = 1) :
        x(x_), y(y_), z(z_), w(w_) {}

    bool operator==(const Vec4& v) const noexcept { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator!=(const Vec4& v) const noexcept { return !(*this == v); }

    static float Dot(const Vec4& a, const Vec4& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
    static Vec4  Cross(const Vec4& a, const Vec4& b) noexcept
    {
        float x = a.y * b.z - a.z * b.y;
        float y = a.z * b.x - a.x * b.z;
        float z = a.x * b.y - a.y * b.x;
        return {x, y, z, 1.0f};
    }
    static Vec4 Normalize(const Vec4& v) noexcept
    {
        float len = v.length();
        return (len > 1e-6f) ? v * (1.0f / len) : Vec4{0, 0, 0, 1.0f};
    }

    constexpr Vec4 operator-() const noexcept { return {-x, -y, -z}; }
    constexpr Vec4 operator+(const Vec4& v) const noexcept { return {x + v.x, y + v.y, z + v.z}; }
    constexpr Vec4 operator-(const Vec4& v) const noexcept { return {x - v.x, y - v.y, z - v.z}; }
    constexpr Vec4 operator*(const Vec4& v) const noexcept { return {x * v.x, y * v.y, z * v.z}; }

    friend constexpr Vec4 operator*(const Vec4& v, float s) noexcept { return {v.x * s, v.y * s, v.z * s}; }
    friend constexpr Vec4 operator*(float s, const Vec4& v) noexcept { return {v.x * s, v.y * s, v.z * s}; }
    friend constexpr Vec4 operator/(const Vec4& v, float s) noexcept { return {v.x / s, v.y / s, v.z / s}; }

    void operator+=(const Vec4& v) noexcept { *this = *this + v; }
    void operator-=(const Vec4& v) noexcept { *this = *this - v; }
    void operator*=(float s) noexcept { *this = *this * s; }
    void operator/=(float s) noexcept { *this = *this / s; }
    void operator*=(const Vec4& v) noexcept { *this = *this * v; } // element-wise multiply

    float lengthSquared() const noexcept { return Dot(*this, *this); }
    float length() const noexcept { return std::sqrt(lengthSquared()); }

    float dot(const Vec4& v) const noexcept { return Dot(*this, v); }
    Vec4  cross(const Vec4& v) const noexcept { return Cross(*this, v); }
    Vec4  normalize() const noexcept { return Normalize(*this); }

    const float* ptr() const noexcept { return &x; }
    float*       ptr() noexcept { return &x; }

    inline bool isEqualWithPrecise(const Vec4& pos) const noexcept { return *this == pos; }
    bool        isEqual(const Vec4& pos) const;
};

} // namespace Voxol::Math

#endif // VOXOL_MATH_VEC2_H