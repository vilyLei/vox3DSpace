#include <iostream>
#include <cmath>
#include "Vec3.h"

namespace voxengine
{
namespace math
{

//template <typename T>
//const T Vec3<T>::s_180OverPi = static_cast<T>(MATH_180_OVER_PI);
template <typename T>
const T Vec3<typename T>::s_minv = getPositiveMinValue<T>();

CREATE_SATATIC_VEC3_INS(T, X_AXIS, 1, 0, 0, 0);
CREATE_SATATIC_VEC3_INS(T, Y_AXIS, 0, 1, 0, 0);
CREATE_SATATIC_VEC3_INS(T, Z_AXIS, 0, 0, 1, 0);
CREATE_SATATIC_VEC3_INS(T, ZERO, 0, 0, 0, 0);
CREATE_SATATIC_VEC3_INS(T, ONE, 1, 1, 1, 1);

template <typename T>
Vec3<T> Vec3<T>::s_v0{};
template <typename T>
Vec3<T> Vec3<T>::s_v1{};

template <typename T>
T& Vec3<T>::operator[](unsigned int i)
{
    return data[i];
}
template <typename T>
T const& Vec3<T>::operator[](unsigned int i) const
{
    return data[i];
}
template <typename T>
void Vec3<T>::set(T px, T py, T pz, T pw)
{
    x = px;
    y = py;
    z = pz;
    w = pw;
}

template <typename T>
void Vec3<T>::setXYZ(T px, T py, T pz)
{
    x = px;
    y = py;
    z = pz;
}

template <typename T>
T Vec3<T>::dot(const Vec3& v3) const
{
    return x * v3.x + y * v3.y + z * v3.z;
}
template <typename T>
T Vec3<T>::getLength() const
{
    return std::sqrt(x * x + y * y + z * z);
}
template <typename T>
T Vec3<T>::getLengthSquared() const
{
    return x * x + y * y + z * z;
}

template <typename T>
void Vec3<T>::copyFrom(const Vec3& v3)
{
    x = v3.x;
    y = v3.y;
    z = v3.z;
}
template <typename T>
void Vec3<T>::multBy(const Vec3& v3)
{
    x *= v3.x;
    y *= v3.y;
    z *= v3.z;
}
template <typename T>
void Vec3<T>::normalize()
{
    T d = sqrt(x * x + y * y + z * z);
    if (d > s_minv)
    {
        x /= d;
        y /= d;
        z /= d;
    }
}
template <typename T>
void Vec3<T>::normalizeTo(Vec3& v3) const
{
    auto d = static_cast<T>( std::sqrt(x * x + y * y + z * z) );
    if (d > s_minv)
    {
        v3.x = x / d;
        v3.y = y / d;
        v3.z = z / d;
    }
    else
    {
        v3.x = x;
        v3.y = y;
        v3.z = z;
    }
}
template <typename T>
void Vec3<T>::scaleVector(const Vec3& v3)
{
    x *= v3.x;
    y *= v3.y;
    z *= v3.z;
}
template <typename T>
void Vec3<T>::scaleBy(T s)
{
    x *= s;
    y *= s;
    z *= s;
}
template <typename T>
void Vec3<T>::negate()
{
    x = -x;
    y = -y;
    z = -z;
}
template <typename T>
bool Vec3<T>::equalsXYZ(const Vec3& v3)
{
    return std::abs(x - v3.x) < s_minv && std::abs(y - v3.y) < s_minv && std::abs(z - v3.z) < s_minv;
}
template <typename T>
bool Vec3<T>::equalsAll(const Vec3& v3)
{
    return std::abs(x - v3.x) < s_minv && std::abs(y - v3.y) < s_minv && std::abs(z - v3.z) < s_minv && std::abs(w - v3.w) < s_minv;
}
template <typename T>
void Vec3<T>::addBy(const Vec3& v3)
{
    x += v3.x;
    y += v3.y;
    z += v3.z;
}
template <typename T>
void Vec3<T>::subtractBy(const Vec3& v3)
{
    x -= v3.x;
    y -= v3.y;
    z -= v3.z;
}
template <typename T>
void Vec3<T>::crossBy(const Vec3& v3)
{
    T px = y * v3.z - z * v3.y;
    T py = z * v3.x - x * v3.z;
    T pz = x * v3.y - y * v3.x;
    x             = px;
    y             = py;
    z             = pz;
}
template <typename T>
void Vec3<T>::reflectBy(const Vec3& nv)
{
    T idotn2 = (x * nv.x + y * nv.y + z * nv.z) * static_cast<T>(2);
    x                 = x - idotn2 * nv.x;
    y                 = y - idotn2 * nv.y;
    z                 = z - idotn2 * nv.z;
}
template <typename T>
void Vec3<typename T>::scaleVecTo(const Vec3& va, T scale)
{
    x = va.x * scale;
    y = va.y * scale;
    z = va.z * scale;
}
template <typename T>
void Vec3<T>::subVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.x - vb.x;
    y = va.y - vb.y;
    z = va.z - vb.z;
}
template <typename T>
void Vec3<T>::addVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.x + vb.x;
    y = va.y + vb.y;
    z = va.z + vb.z;
}
template <typename T>
void Vec3<T>::crossVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.y * vb.z - va.z * vb.y;
    y = va.z * vb.x - va.x * vb.z;
    z = va.x * vb.y - va.y * vb.x;
}
template <typename T>
Vec3<T> Vec3<T>::subtract(const Vec3& v3) const
{
    return Vec3<T>(x - v3.x, y - v3.y, z - v3.z);
}
template <typename T>
Vec3<T> Vec3<T>::crossProduct(const Vec3& v3) const
{
    return Vec3<T>(y * v3.z - z * v3.y, z * v3.x - x * v3.z, x * v3.y - y * v3.x);
}
template <typename T>
Vec3<T> Vec3<T>::clone() const
{
    return Vec3<T>(x, y, z, w);
}


template <typename T>
Vec3<T>& Vec3<T>::toArray3(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        std::memcpy(arr + offset, data, sizeof(T) * 3);
    }
    return *this;
}
template <typename T>
Vec3<T>& Vec3<T>::toArray4(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        std::memcpy(arr + offset, data, sizeof(T) << 2);
    }
    return *this;
}

template <typename T>
Vec3<T>& Vec3<typename T>::fromArray3(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        std::memcpy(data, arr + offset, sizeof(T) * 3);
    }
    return *this;
}

template <typename T>
Vec3<T>& Vec3<typename T>::fromArray4(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        std::memcpy(data, arr + offset, sizeof(T) << 2);
    }
    return *this;
}


template <typename T>
std::string Vec3<T>::toString() const
{
    return "Vec3(x=" + std::to_string(x) + ",y=" + std::to_string(y) + ",z=" + std::to_string(z) + ",w=" + std::to_string(w) + ")";
}

template <typename T>
void Vec3<T>::cross(const Vec3& a, const Vec3& b, Vec3& result)
{
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
}
// (va1 - va0) 叉乘 (vb1 - vb0), 右手法则(为正)
template <typename T>
void Vec3<T>::crossSubtract(const Vec3& va0, const Vec3& va1, const Vec3& vb0, const Vec3& vb1, Vec3& result)
{
    s_v0.x = va1.x - va0.x;
    s_v0.y = va1.y - va0.y;
    s_v0.z = va1.z - va0.z;

    s_v1.x   = vb1.x - vb0.x;
    s_v1.y   = vb1.y - vb0.y;
    s_v1.z   = vb1.z - vb0.z;
    auto& v0 = s_v0;
    auto& v1 = s_v1;
    result.x = v0.y * v1.z - v0.z * v1.y;
    result.y = v0.z * v1.x - v0.x * v1.z;
    result.z = v0.x * v1.y - v0.y * v1.x;
}
template <typename T>
void Vec3<T>::subtract(const Vec3& a, const Vec3& b, Vec3& result)
{
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
}
template <typename T>
T Vec3<T>::distanceSquared(const Vec3& a, const Vec3& b)
{
    s_v0.x = a.x - b.x;
    s_v0.y = a.y - b.y;
    s_v0.z = a.z - b.z;
    return s_v0.getLengthSquared();
}
template <typename T>
T Vec3<T>::distanceXYZ(T x0, T y0, T z0, T x1, T y1, T z1)
{
    s_v0.x = x0 - x1;
    s_v0.y = y0 - y1;
    s_v0.z = z0 - z1;
    return s_v0.getLength();
}
template <typename T>
T Vec3<T>::distance(const Vec3& v0, const Vec3& v1)
{
    s_v0.x = v0.x - v1.x;
    s_v0.y = v0.y - v1.y;
    s_v0.z = v0.z - v1.z;
    return s_v0.getLength();
}

/**
  * get angle degree between two Vec3 objects
  * @param v0 src Vec3 object
  * @param v1 dst Vec3 object
  * @returns angle degree
  */
template <typename T>
T Vec3<T>::angleBetween(const Vec3& v0, const Vec3& v1)
{
    v0.normalizeTo(s_v0);
    v1.normalizeTo(s_v1);
    return std::acos(s_v0.dot(s_v1)) * s_180OverPi;
}
/**
  * get angle radian between two Vec3 objects
  * @param v0 src Vec3 object
  * @param v1 dst Vec3 object
  * @returns angle radian
  */
template <typename T>
T Vec3<T>::radianBetween(const Vec3& v0, const Vec3& v1)
{
    v0.normalizeTo(s_v0);
    v1.normalizeTo(s_v1);
    return std::acos(s_v0.dot(s_v1));
}

template <typename T>
T Vec3<T>::radianBetween2(const Vec3& v0, const Vec3& v1)
{
    //  c^2 = a^2 + b^2 - 2*a*b * cos(x)
    //  cos(x) = (a^2 + b^2 - c^2) / 2*a*b
    auto pa = v0.getLengthSquared();
    auto pb = v1.getLengthSquared();
    s_v0.x   = v0.x - v1.x;
    s_v0.y   = v0.y - v1.y;
    s_v0.z   = v0.z - v1.z;
    return std::acos((pa + pb - s_v0.getLengthSquared()) / (static_cast<T>(2) * std::sqrt(pa) * std::sqrt(pb)));
}
template <typename T>
void Vec3<T>::reflect(const Vec3& iv, const Vec3& nv, Vec3& rv)
{
    auto idotn2  = (iv.x * nv.x + iv.y * nv.y + iv.z * nv.z) * static_cast<T>(2);
    rv.x         = iv.x - idotn2 * nv.x;
    rv.y         = iv.y - idotn2 * nv.y;
    rv.z         = iv.z - idotn2 * nv.z;
}
template class Vec3<double>;
template class Vec3<float>;
template class Vec3<char>;
template class Vec3<short>;
template class Vec3<int>;
template class Vec3<long>;

} // namespace math
} // namespace voxengine