#pragma once
#include <string>
#include "mathDefine.h"
namespace voxengine
{
namespace math
{
template <typename T>
class Vec3
{
public:
    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        T data[4];

        //#ifdef VOXENGINE_SIMD
        //        __m128 sd4;
        //#endif // VOXENGINE_SIMD
    };

    ~Vec3() = default;

    constexpr Vec3(T x, T y, T z, T w = static_cast<T>(0)) noexcept :
        x(x), y(y), z(z), w(w)
    {}
    Vec3()                         = default;
    Vec3(const Vec3& v)            = default;
    Vec3(Vec3&& v)                 = default;
    Vec3& operator=(const Vec3& v) = default;
    Vec3& operator=(Vec3&& v)      = default;

    bool operator==(const Vec3& v) const
    {
        return equalsAll(v);
    }
    Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vec3& operator-=(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Vec3& operator*=(const Vec3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vec3& operator/=(const Vec3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    Vec3 operator+(const Vec3& v)
    {
        return Vec3(
            x + v.x,
            y + v.y,
            z + v.z);
    }
    Vec3 operator-(const Vec3& v)
    {
        return Vec3(
            x - v.x,
            y - v.y,
            z - v.z);
    }
    Vec3 operator*(const Vec3& v)
    {
        return Vec3(
            x * v.x,
            y * v.y,
            z * v.z);
    }
    Vec3 operator/(const Vec3& v)
    {
        return Vec3(
            x / v.x,
            y / v.y,
            z / v.z);
    }

    T&       operator[](unsigned int i);
    T const& operator[](unsigned int i) const;
    void     set(T px, T py, T pz, T pw);
    void     setXYZ(T px, T py, T pz);

    T dot(const Vec3& v3) const;
    T getLength() const;
    T getLengthSquared() const;

    void copyFrom(const Vec3& v3);
    void multBy(const Vec3& v3);
    void normalize();
    void normalizeTo(Vec3& v3) const;
    void scaleVector(const Vec3& v3);
    void scaleBy(T s);
    void negate();
    bool equalsXYZ(const Vec3& v3);
    bool equalsAll(const Vec3& v3) const;
    void addBy(const Vec3& v3);
    void subtractBy(const Vec3& v3);
    void crossBy(const Vec3& v3);
    void reflectBy(const Vec3& nv);

    void scaleVecTo(const Vec3& va, T scale);
    void subVecsTo(const Vec3& va, const Vec3& vb);

    void addVecsTo(const Vec3& va, const Vec3& vb);
    void crossVecsTo(const Vec3& va, const Vec3& vb);
    Vec3 subtract(const Vec3& v3) const;
    Vec3 crossProduct(const Vec3& v3) const;
    Vec3 clone() const;

    Vec3&       toArray3(T* arr, unsigned int offset = 0);
    Vec3&       toArray4(T* arr, unsigned int offset = 0);
    Vec3&       fromArray3(T* arr, unsigned int offset = 0);
    Vec3&       fromArray4(T* arr, unsigned int offset = 0);
    std::string toString() const;


    /**
     * ���ַ���(Ϊ��)
     */
    static void cross(const Vec3& a, const Vec3& b, Vec3& result);
    // (va1 - va0) ��� (vb1 - vb0), ���ַ���(Ϊ��)
    static void crossSubtract(const Vec3& va0, const Vec3& va1, const Vec3& vb0, const Vec3& vb1, Vec3& result);
    static void subtract(const Vec3& a, const Vec3& b, Vec3& result);
    static T    distanceSquared(const Vec3& a, const Vec3& b);
    static T    distanceXYZ(T x0, T y0, T z0, T x1, T y1, T z1);
    static T    distance(const Vec3& v0, const Vec3& v1);

    /**
     * get angle degree between two Vec3 objects
     * @param v0 src Vec3 object
     * @param v1 dst Vec3 object
     * @returns angle degree
     */
    static T angleBetween(const Vec3& v0, const Vec3& v1);
    /**
     * get angle radian between two Vec3 objects
     * @param v0 src Vec3 object
     * @param v1 dst Vec3 object
     * @returns angle radian
     */
    static T    radianBetween(const Vec3& v0, const Vec3& v1);
    static T    radianBetween2(const Vec3& v0, const Vec3& v1);
    static void reflect(const Vec3& iv, const Vec3& nv, Vec3& rv);

    const static Vec3 X_AXIS;
    const static Vec3 Y_AXIS;
    const static Vec3 Z_AXIS;
    const static Vec3 ZERO;
    const static Vec3 ONE;

private:
    inline const static T s_180OverPi = static_cast<T>(MATH_180_OVER_PI);
    inline const static T s_minv      = getPositiveMinValue<T>();
    static Vec3           s_v0;
    static Vec3           s_v1;
};

typedef Vec3<long> UVec3;


#define CREATE_STATIC_VEC3_INS(T, NAME, v0, v1, v2, v3) \
    template <typename T>                                \
    const Vec3<T> Vec3<T>::NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2), static_cast<T>(v3)};

namespace vec3
{
#define CREATE_S_VEC3_INS(T, NAME, v0, v1, v2, v3) \
    template <typename T>                                \
    constexpr Vec3<T> NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2), static_cast<T>(v3)};

using namespace voxengine::math;
CREATE_S_VEC3_INS(T, X_AXIS, 1, 0, 0, 0);
CREATE_S_VEC3_INS(T, Y_AXIS, 0, 1, 0, 0);
CREATE_S_VEC3_INS(T, Z_AXIS, 0, 0, 1, 0);
CREATE_S_VEC3_INS(T, ZERO, 0, 0, 0, 0);
CREATE_S_VEC3_INS(T, ONE, 1, 1, 1, 1);
} // namespace vec3
} // namespace math
} // namespace voxengine