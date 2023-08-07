#include <iostream>
#include <cmath>
#include "mathDefine.h"
#include "Vec3.h"

namespace voxengine
{
namespace math
{

template <typename NumberType>
const NumberType Vec3<NumberType>::s_180OverPi = static_cast<NumberType>(MATH_180_OVER_PI);
template <typename NumberType>
const NumberType Vec3<typename NumberType>::s_minv = getPositiveMinValue<NumberType>();

CREATE_SATATIC_VEC3_INS(NumberType, X_AXIS, 1, 0, 0, 0);
CREATE_SATATIC_VEC3_INS(NumberType, Y_AXIS, 0, 1, 0, 0);
CREATE_SATATIC_VEC3_INS(NumberType, Z_AXIS, 0, 0, 1, 0);
CREATE_SATATIC_VEC3_INS(NumberType, ZERO, 0, 0, 0, 0);
CREATE_SATATIC_VEC3_INS(NumberType, ONE, 1, 1, 1, 1);

template <typename NumberType>
Vec3<NumberType> Vec3<NumberType>::s_v0{};
template <typename NumberType>
Vec3<NumberType> Vec3<NumberType>::s_v1{};


template <typename NumberType>
Vec3<NumberType>::Vec3(NumberType px, NumberType py, NumberType pz, NumberType pw) noexcept
    :
    x(px), y(py), z(pz), w(pw)
{
}
template <typename NumberType>
Vec3<NumberType>::Vec3() noexcept
    :
    x(static_cast<NumberType>(0)), y(static_cast<NumberType>(0)), z(static_cast<NumberType>(0)), w(static_cast<NumberType>(1))
{
}

template <typename NumberType>
NumberType& Vec3<NumberType>::operator[](unsigned int i)
{

    switch (i)
    {
        case 1:
            return y;
            break;
        case 2:
            return z;
            break;
        case 3:
            return w;
            break;
        default:
            break;
    }
    return x;
}

template <typename NumberType>
void Vec3<NumberType>::set(NumberType px, NumberType py, NumberType pz, NumberType pw)
{
    x = px;
    y = py;
    z = pz;
    w = pw;
}

template <typename NumberType>
void Vec3<NumberType>::setXYZ(NumberType px, NumberType py, NumberType pz)
{
    x = px;
    y = py;
    z = pz;
}

template <typename NumberType>
NumberType Vec3<NumberType>::dot(const Vec3& v3) const
{
    return x * v3.x + y * v3.y + z * v3.z;
}
template <typename NumberType>
NumberType Vec3<NumberType>::getLength() const
{
    return std::sqrt(x * x + y * y + z * z);
}
template <typename NumberType>
NumberType Vec3<NumberType>::getLengthSquared() const
{
    return x * x + y * y + z * z;
}

template <typename NumberType>
void Vec3<NumberType>::copyFrom(const Vec3& v3)
{
    x = v3.x;
    y = v3.y;
    z = v3.z;
}
template <typename NumberType>
void Vec3<NumberType>::multBy(const Vec3& v3)
{
    x *= v3.x;
    y *= v3.y;
    z *= v3.z;
}
template <typename NumberType>
void Vec3<NumberType>::normalize()
{
    NumberType d = sqrt(x * x + y * y + z * z);
    if (d > s_minv)
    {
        x /= d;
        y /= d;
        z /= d;
    }
}
template <typename NumberType>
void Vec3<NumberType>::normalizeTo(Vec3& v3) const
{
    auto d = static_cast<NumberType>( std::sqrt(x * x + y * y + z * z) );
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
template <typename NumberType>
void Vec3<NumberType>::scaleVector(const Vec3& v3)
{
    x *= v3.x;
    y *= v3.y;
    z *= v3.z;
}
template <typename NumberType>
void Vec3<NumberType>::scaleBy(NumberType s)
{
    x *= s;
    y *= s;
    z *= s;
}
template <typename NumberType>
void Vec3<NumberType>::negate()
{
    x = -x;
    y = -y;
    z = -z;
}
template <typename NumberType>
bool Vec3<NumberType>::equalsXYZ(const Vec3& v3)
{
    return std::abs(x - v3.x) < s_minv && std::abs(y - v3.y) < s_minv && std::abs(z - v3.z) < s_minv;
}
template <typename NumberType>
bool Vec3<NumberType>::equalsAll(const Vec3& v3)
{
    return std::abs(x - v3.x) < s_minv && std::abs(y - v3.y) < s_minv && std::abs(z - v3.z) < s_minv && std::abs(w - v3.w) < s_minv;
}
template <typename NumberType>
void Vec3<NumberType>::project()
{
    NumberType t = static_cast<NumberType>(1) / w;
    x *= t;
    y *= t;
    z *= t;
}
template <typename NumberType>
void Vec3<NumberType>::addBy(const Vec3& v3)
{
    x += v3.x;
    y += v3.y;
    z += v3.z;
}
template <typename NumberType>
void Vec3<NumberType>::subtractBy(const Vec3& v3)
{
    x -= v3.x;
    y -= v3.y;
    z -= v3.z;
}
template <typename NumberType>
void Vec3<NumberType>::crossBy(const Vec3& v3)
{
    NumberType px = y * v3.z - z * v3.y;
    NumberType py = z * v3.x - x * v3.z;
    NumberType pz = x * v3.y - y * v3.x;
    x             = px;
    y             = py;
    z             = pz;
}
template <typename NumberType>
void Vec3<NumberType>::reflectBy(const Vec3& nv)
{
    NumberType idotn2 = (x * nv.x + y * nv.y + z * nv.z) * static_cast<NumberType>(2);
    x                 = x - idotn2 * nv.x;
    y                 = y - idotn2 * nv.y;
    z                 = z - idotn2 * nv.z;
}
template <typename NumberType>
void Vec3<typename NumberType>::scaleVecTo(const Vec3& va, NumberType scale)
{
    x = va.x * scale;
    y = va.y * scale;
    z = va.z * scale;
}
template <typename NumberType>
void Vec3<NumberType>::subVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.x - vb.x;
    y = va.y - vb.y;
    z = va.z - vb.z;
}
template <typename NumberType>
void Vec3<NumberType>::addVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.x + vb.x;
    y = va.y + vb.y;
    z = va.z + vb.z;
}
template <typename NumberType>
void Vec3<NumberType>::crossVecsTo(const Vec3& va, const Vec3& vb)
{
    x = va.y * vb.z - va.z * vb.y;
    y = va.z * vb.x - va.x * vb.z;
    z = va.x * vb.y - va.y * vb.x;
}
template <typename NumberType>
Vec3<NumberType> Vec3<NumberType>::subtract(const Vec3& v3) const
{
    return Vec3<NumberType>(x - v3.x, y - v3.y, z - v3.z);
}
template <typename NumberType>
Vec3<NumberType> Vec3<NumberType>::crossProduct(const Vec3& v3) const
{
    return Vec3<NumberType>(y * v3.z - z * v3.y, z * v3.x - x * v3.z, x * v3.y - y * v3.x);
}
template <typename NumberType>
Vec3<NumberType> Vec3<NumberType>::clone() const
{
    return Vec3<NumberType>(x, y, z, w);
}


template <typename NumberType>
void Vec3<NumberType>::toArray3(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0]   = x;
        ls[1]   = y;
        ls[2]   = z;
    }
}
template <typename NumberType>
void Vec3<NumberType>::toArray4(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0] = x;
        ls[1] = y;
        ls[2] = z;
        ls[3] = w;
    }
}

template <typename NumberType>
Vec3<NumberType>* Vec3<typename NumberType>::fromArray3(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x = ls[0];
        y = ls[1];
        z = ls[2];
    }
    return this;
}

template <typename NumberType>
Vec3<NumberType>* Vec3<typename NumberType>::fromArray4(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x       = ls[0];
        y       = ls[1];
        z       = ls[2];
        w       = ls[3];
    }
    return this;
}


template <typename NumberType>
std::string Vec3<NumberType>::toString() const
{
    return "Vec3(x=" + std::to_string(x) + ",y=" + std::to_string(y) + ",z=" + std::to_string(z) + ",w=" + std::to_string(w) + ")";
}

template <typename NumberType>
void Vec3<NumberType>::cross(const Vec3& a, const Vec3& b, Vec3& result)
{
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
}
// (va1 - va0) 叉乘 (vb1 - vb0), 右手法则(为正)
template <typename NumberType>
void Vec3<NumberType>::crossSubtract(const Vec3& va0, const Vec3& va1, const Vec3& vb0, const Vec3& vb1, Vec3& result)
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
template <typename NumberType>
void Vec3<NumberType>::subtract(const Vec3& a, const Vec3& b, Vec3& result)
{
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
}
template <typename NumberType>
NumberType Vec3<NumberType>::distanceSquared(const Vec3& a, const Vec3& b)
{
    s_v0.x = a.x - b.x;
    s_v0.y = a.y - b.y;
    s_v0.z = a.z - b.z;
    return s_v0.getLengthSquared();
}
template <typename NumberType>
NumberType Vec3<NumberType>::distanceXYZ(NumberType x0, NumberType y0, NumberType z0, NumberType x1, NumberType y1, NumberType z1)
{
    s_v0.x = x0 - x1;
    s_v0.y = y0 - y1;
    s_v0.z = z0 - z1;
    return s_v0.getLength();
}
template <typename NumberType>
NumberType Vec3<NumberType>::distance(const Vec3& v0, const Vec3& v1)
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
template <typename NumberType>
NumberType Vec3<NumberType>::angleBetween(const Vec3& v0, const Vec3& v1)
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
template <typename NumberType>
NumberType Vec3<NumberType>::radianBetween(const Vec3& v0, const Vec3& v1)
{
    v0.normalizeTo(s_v0);
    v1.normalizeTo(s_v1);
    return std::acos(s_v0.dot(s_v1));
}

template <typename NumberType>
NumberType Vec3<NumberType>::radianBetween2(const Vec3& v0, const Vec3& v1)
{
    //  c^2 = a^2 + b^2 - 2*a*b * cos(x)
    //  cos(x) = (a^2 + b^2 - c^2) / 2*a*b
    auto pa = v0.getLengthSquared();
    auto pb = v1.getLengthSquared();
    s_v0.x   = v0.x - v1.x;
    s_v0.y   = v0.y - v1.y;
    s_v0.z   = v0.z - v1.z;
    return std::acos((pa + pb - s_v0.getLengthSquared()) / (static_cast<NumberType>(2) * std::sqrt(pa) * std::sqrt(pb)));
}
template <typename NumberType>
void Vec3<NumberType>::reflect(const Vec3& iv, const Vec3& nv, Vec3& rv)
{
    auto idotn2  = (iv.x * nv.x + iv.y * nv.y + iv.z * nv.z) * static_cast<NumberType>(2);
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