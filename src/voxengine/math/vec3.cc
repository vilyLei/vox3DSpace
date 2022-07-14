#include <iostream>
#include <cmath>
#include "mathDefine.h"
#include "Vec3.h"

namespace voxengine
{
namespace math
{

template <typename NumberType>
const NumberType Vec3<typename NumberType>::s_180OverPi = static_cast<NumberType>(MATH_180_OVER_PI);
template <typename NumberType>
const NumberType Vec3<typename NumberType>::s_minv = getPositiveMinValue<NumberType>();

template <typename NumberType>
const Vec3<typename NumberType> Vec3<typename NumberType>::X_AXIS{1.0f, 0.0f, 0.0f, 0.0};
template <typename NumberType>
const Vec3<typename NumberType> Vec3<typename NumberType>::Y_AXIS{0.0f, 1.0f, 0.0f, 0.0};
template <typename NumberType>
const Vec3<typename NumberType> Vec3<typename NumberType>::Z_AXIS{0.0f, 1.0f, 0.0f, 0.0};
template <typename NumberType>
const Vec3<typename NumberType> Vec3<typename NumberType>::ZERO{0.0f, 0.0f, 0.0f, 0.0};
template <typename NumberType>
const Vec3<typename NumberType> Vec3<typename NumberType>::ONE{1.0f, 1.0f, 1.0f, 1.0};

template <typename NumberType>
Vec3<typename NumberType> Vec3<typename NumberType>::s_v0{};
template <typename NumberType>
Vec3<typename NumberType> Vec3<typename NumberType>::s_v1{};


template <typename NumberType>
Vec3<typename NumberType>::Vec3(NumberType px, NumberType py, NumberType pz, NumberType pw) noexcept
    :
    x(px), y(py), z(pz), w(pw)
{
    //std::cout << "Vec3::constructor() ..." << std::endl;
}
template <typename NumberType>
Vec3<typename NumberType>::Vec3() noexcept
    :
    x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
    //std::cout << "Vec3::constructor() default..." << std::endl;
    //std::cout << std::boolalpha
    //std::cout << "sizeof(NumberType): " << sizeof(NumberType) << ", s_minv: " << s_minv << std::endl;
    //std::cout << "0.0000001 > s_minv: " << (0.0000001 > s_minv) << std::endl;
}
//template <typename NumberType>
//Vec3<typename NumberType>::~Vec3()
//{
//    //std::cout << "Vec3::deconstructor()..." << std::endl;
//}

template <typename NumberType>
NumberType& Vec3<typename NumberType>::operator[](unsigned int i)
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
NumberType Vec3<typename NumberType>::dot(const Vec3& v3) const
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
    NumberType t = 1.0f / w;
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
    NumberType idotn2 = (x * nv.x + y * nv.y + z * nv.z) * 2.0f;
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
Vec3<NumberType> Vec3<typename NumberType>::subtract(const Vec3& v3) const
{
    return Vec3<NumberType>(x - v3.x, y - v3.y, z - v3.z);
}
template <typename NumberType>
Vec3<NumberType> Vec3<typename NumberType>::crossProduct(const Vec3& v3) const
{
    return Vec3<NumberType>(y * v3.z - z * v3.y, z * v3.x - x * v3.z, x * v3.y - y * v3.x);
}
template <typename NumberType>
Vec3<NumberType> Vec3<typename NumberType>::clone() const
{
    return Vec3<NumberType>(x, y, z, w);
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
std::string Vec3<NumberType>::toString()
{
    return "Vec3(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(w) + ")";
}

template <typename NumberType>
void Vec3<NumberType>::cross(const Vec3& a, const Vec3& b, Vec3& result)
{
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
}
// (va1 - va0) ��� (vb1 - vb0), ���ַ���(Ϊ��)
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
  * get angle degree between two Vector3D objects
  * @param v0 src Vector3D object
  * @param v1 dst Vector3D object
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
  * get angle radian between two Vector3D objects
  * @param v0 src Vector3D object
  * @param v1 dst Vector3D object
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
    //  // c^2 = a^2 + b^2 - 2*a*b * cos(x)
    //  // cos(x) = (a^2 + b^2 - c^2) / 2*a*b
    auto pa = v0.getLengthSquared();
    auto pb = v1.getLengthSquared();
    s_v0.x   = v0.x - v1.x;
    s_v0.y   = v0.y - v1.y;
    s_v0.z   = v0.z - v1.z;
    return std::acos((pa + pb - s_v0.getLengthSquared()) / (2.0f * std::sqrt(pa) * std::sqrt(pb)));
}
template <typename NumberType>
void Vec3<NumberType>::reflect(const Vec3& iv, const Vec3& nv, Vec3& rv)
{
    auto idotn2 = (iv.x * nv.x + iv.y * nv.y + iv.z * nv.z) * 2.0f;
    rv.x         = iv.x - idotn2 * nv.x;
    rv.y         = iv.y - idotn2 * nv.y;
    rv.z         = iv.z - idotn2 * nv.z;
}

template <typename NumberType>
void __$templateConstructVec3(NumberType value)
{
    Vec3<NumberType> va{};
    Vec3<NumberType> v{};

    NumberType vs[4]{value, value, value, value};

    v[0] += value;
    v.set(value, value, value, value);
    v.setXYZ(value, value, value);

    v.dot(va);
    v.getLength();
    v.getLengthSquared();

    v.copyFrom(va);
    v.multBy(va);
    v.normalize();
    v.normalizeTo(va);
    v.scaleVector(va);
    v.scaleBy(value);
    v.negate();
    v.equalsXYZ(va);
    v.equalsAll(va);
    v.project();
    v.addBy(va);
    v.subtractBy(va);
    v.crossBy(va);
    v.reflectBy(va);

    v.scaleVecTo(va, value);
    v.subVecsTo(va, va);
    v.addVecsTo(va, va);
    v.crossVecsTo(va, va);
    v.subtract(va);
    v.crossProduct(va);

    auto v1 = v.clone();

    v.fromArray3(vs);
    v.fromArray4(vs);

    v.toString();

    auto pv0 = Vec3<NumberType>::X_AXIS;
    auto pv1 = Vec3<NumberType>::Y_AXIS;
    auto pv2 = Vec3<NumberType>::Z_AXIS;
    auto pv3 = Vec3<NumberType>::ONE;
    auto pv4 = Vec3<NumberType>::ZERO;

    Vec3<NumberType>::cross(va, va, va);
    Vec3<NumberType>::crossSubtract(va, va, va, va, va);
    Vec3<NumberType>::subtract(va, va, va);
    Vec3<NumberType>::distanceSquared(va, va);
    Vec3<NumberType>::distanceXYZ(value, value, value, value, value, value);
    Vec3<NumberType>::distance(va, va);
    Vec3<NumberType>::angleBetween(va, va);
    Vec3<NumberType>::radianBetween(va, va);
    Vec3<NumberType>::radianBetween2(va, va);
    Vec3<NumberType>::reflect(va, va, va);
}
void __$templateImplyVec3()
{
    __$templateConstructVec3(1.0);
    __$templateConstructVec3(1.0f);
}

} // namespace math
} // namespace voxengine