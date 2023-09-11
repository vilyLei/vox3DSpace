#pragma once
#include <string>
#include "mathDefine.h"
namespace voxengine
{
namespace math
{
template <typename T>
class Vec2
{
public:
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        T data[3];
    };

    ~Vec2() = default;

    constexpr Vec2(T x, T y, T z = static_cast<T>(0)) noexcept :
        x(x), y(y), z(z)
    {}
    Vec2()                         = default;
    Vec2(const Vec2& v)            = default;
    Vec2(Vec2&& v)                 = default;
    Vec2& operator=(const Vec2& v) = default;
    Vec2& operator=(Vec2&& v)      = default;

    Vec2& operator+=(const Vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vec2& operator-=(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vec2& operator*=(const Vec2& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }
    Vec2& operator/=(const Vec2& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    Vec2 operator+(const Vec2& v)
    {
        return Vec2(
            x + v.x,
            y + v.y);
    }
    Vec2 operator-(const Vec2& v)
    {
        return Vec2(
            x - v.x,
            y - v.y);
    }
    Vec2 operator*(const Vec2& v)
    {
        return Vec2(
            x * v.x,
            y * v.y);
    }
    Vec2 operator/(const Vec2& v)
    {
        return Vec2(
            x / v.x,
            y / v.y);
    }

    T&       operator[](unsigned int i);
    T const& operator[](unsigned int i) const;
    void     set(T px, T py, T pz);
    void     setXY(T px, T py);

    T dot(const Vec2& v2) const;
    T getLength() const;
    T getLengthSquared() const;

    void copyFrom(const Vec2& v2);
    void multBy(const Vec2& v2);
    void normalize();
    void normalizeTo(Vec2& v2) const;
    void scaleVector(const Vec2& v2);
    void scaleBy(T s);
    void negate();
    bool equalsXYZ(const Vec2& v2);
    bool equalsAll(const Vec2& v2);
    void addBy(const Vec2& v2);
    void subtractBy(const Vec2& v2);
    T    crossBy(const Vec2& v2) const;
    void reflectBy(const Vec2& nv);

    void scaleVecTo(const Vec2& va, T scale);
    void subVecsTo(const Vec2& va, const Vec2& vb);

    void addVecsTo(const Vec2& va, const Vec2& vb);
    Vec2 subtract(const Vec2& v2) const;
    Vec2 clone() const;

    void        toArray2(T* arr, unsigned int offset = 0);
    void        toArray3(T* arr, unsigned int offset = 0);
    Vec2*       fromArray2(T* arr, unsigned int offset = 0);
    Vec2*       fromArray3(T* arr, unsigned int offset = 0);
    std::string toString() const;

    const static Vec2 X_AXIS;
    const static Vec2 Y_AXIS;
    const static Vec2 ZERO;
    const static Vec2 ONE;

private:
    inline const static T s_180OverPi = static_cast<T>(MATH_180_OVER_PI);
    inline const static T        s_minv = getPositiveMinValue<T>();
    static Vec2           s_v0;
    static Vec2           s_v1;
};

typedef Vec2<long> UVec2;


#define CREATE_SATATIC_VEC2_INS(T, NAME, v0, v1, v2) \
    template <typename T>                            \
    const Vec2<T> Vec2<T>::NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2)};

} // namespace math
} // namespace voxengine