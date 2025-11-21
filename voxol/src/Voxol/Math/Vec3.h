
#ifndef VOXOL_MATH_VEC3_H
#define VOXOL_MATH_VEC3_H

#include <cmath>

namespace Voxol::Math
{
struct Vec3
{
    union
    {
        struct
        {
            float x, y, z;
        };
        float data[3];
    };
    constexpr Vec3() noexcept :
        data{} {}
    constexpr Vec3(float x_, float y_, float z_) :
        x(x_), y(y_), z(z_) {}

    bool operator==(const Vec3& v) const noexcept { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3& v) const noexcept { return !(*this == v); }

    static float Dot(const Vec3& a, const Vec3& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static Vec3  Cross(const Vec3& a, const Vec3& b) noexcept
    {

        float x = a.y * b.z - a.z * b.y;
        float y = a.z * b.x - a.x * b.z;
        float z = a.x * b.y - a.y * b.x;
        return {x, y, z};
    }
    static Vec3 Normalize(const Vec3& v) noexcept
    {
        float len = v.length();
        return (len > 1e-6f) ? v * (1.0f / len) : Vec3{0, 0, 0};
    }

    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }
    constexpr Vec3 operator+(const Vec3& v) const noexcept { return {x + v.x, y + v.y, z + v.z}; }
    constexpr Vec3 operator-(const Vec3& v) const noexcept { return {x - v.x, y - v.y, z - v.z}; }
    constexpr Vec3 operator*(const Vec3& v) const noexcept { return {x * v.x, y * v.y, z * v.z}; }

    friend constexpr Vec3 operator*(const Vec3& v, float s) noexcept { return {v.x * s, v.y * s, v.z * s}; }
    friend constexpr Vec3 operator*(float s, const Vec3& v) noexcept { return {v.x * s, v.y * s, v.z * s}; }
    friend constexpr Vec3 operator/(const Vec3& v, float s) noexcept { return {v.x / s, v.y / s, v.z / s}; }

    void operator+=(const Vec3& v) noexcept { *this = *this + v; }
    void operator-=(const Vec3& v) noexcept { *this = *this - v; }
    void operator*=(float s) noexcept { *this = *this * s; }
    void operator/=(float s) noexcept { *this = *this / s; }
    void operator*=(const Vec3& v) noexcept { *this = *this * v; } // element-wise multiply

    float lengthSquared() const noexcept { return Dot(*this, *this); }
    float length() const noexcept { return std::sqrt(lengthSquared()); }

    float dot(const Vec3& v) const noexcept { return Dot(*this, v); }
    Vec3  cross(const Vec3& v) const noexcept { return Cross(*this, v); }
    Vec3  normalize() const noexcept { return Normalize(*this); }

    const float* ptr() const noexcept { return &x; }
    float*       ptr() noexcept { return &x; }

    inline bool isEqualWithPrecise(const Vec3& pos) const noexcept { return *this == pos; }
    bool        isEqual(const Vec3& pos) const;
};

} // namespace Voxol::Math

#endif // VOXOL_MATH_VEC2_H