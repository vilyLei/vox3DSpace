#include <iostream>
#include <cmath>
#include "Vec2.h"

namespace voxengine
{
namespace math
{

//template <typename T>
//const T Vec2<T>::s_180OverPi = static_cast<T>(MATH_180_OVER_PI);
//template <typename T>
//const T Vec2<typename T>::s_minv = getPositiveMinValue<T>();

CREATE_SATATIC_VEC2_INS(T, X_AXIS, 1, 0, 0);
CREATE_SATATIC_VEC2_INS(T, Y_AXIS, 0, 1, 0);
CREATE_SATATIC_VEC2_INS(T, ZERO, 0, 0, 0);
CREATE_SATATIC_VEC2_INS(T, ONE, 1, 1, 1);

template <typename T>
Vec2<T> Vec2<T>::s_v0{};
template <typename T>
Vec2<T> Vec2<T>::s_v1{};

template <typename T>
T& Vec2<T>::operator[](unsigned int i)
{
    return data[i];
}
template <typename T>
T const& Vec2<T>::operator[](unsigned int i) const
{
    return data[i];
}
template <typename T>
void Vec2<T>::set(T px, T py, T pz)
{
    x = px;
    y = py;
    z = pz;
}

template <typename T>
void Vec2<T>::setXY(T px, T py)
{
    x = px;
    y = py;
}

template <typename T>
T Vec2<T>::dot(const Vec2& v2) const
{
    return x * v2.x + y * v2.y;
}
template <typename T>
T Vec2<T>::getLength() const
{
    return std::sqrt(x * x + y * y);
}
template <typename T>
T Vec2<T>::getLengthSquared() const
{
    return x * x + y * y;
}

template <typename T>
void Vec2<T>::copyFrom(const Vec2& v2)
{
    x = v2.x;
    y = v2.y;
}
template <typename T>
void Vec2<T>::multBy(const Vec2& v2)
{
    x *= v2.x;
    y *= v2.y;
}
template <typename T>
void Vec2<T>::normalize()
{
    T d = sqrt(x * x + y * y);
    if (d > s_minv)
    {
        x /= d;
        y /= d;
    }
}
template <typename T>
void Vec2<T>::normalizeTo(Vec2& v2) const
{
    auto d = std::sqrt(x * x + y * y);
    if (d > s_minv)
    {
        v2.x = x / d;
        v2.y = y / d;
    }
    else
    {
        v2.x = x;
        v2.y = y;
    }
}
template <typename T>
void Vec2<T>::scaleVector(const Vec2& v2)
{
    x *= v2.x;
    y *= v2.y;
}
template <typename T>
void Vec2<T>::scaleBy(T s)
{
    x *= s;
    y *= s;
}
template <typename T>
void Vec2<T>::negate()
{
    x = -x;
    y = -y;
}
template <typename T>
bool Vec2<T>::equalsXYZ(const Vec2& v2)
{
    return std::abs(x - v2.x) < s_minv && std::abs(y - v2.y) < s_minv;
}
template <typename T>
bool Vec2<T>::equalsAll(const Vec2& v2)
{
    return std::abs(x - v2.x) < s_minv && std::abs(y - v2.y) < s_minv && std::abs(z - v2.z) < s_minv;
}

template <typename T>
void Vec2<T>::addBy(const Vec2& v2)
{
    x += v2.x;
    y += v2.y;
}
template <typename T>
void Vec2<T>::subtractBy(const Vec2& v2)
{
    x -= v2.x;
    y -= v2.y;
}
template <typename T>
void Vec2<T>::reflectBy(const Vec2& nv)
{
    T idotn2 = (x * nv.x + y * nv.y) * static_cast<T>(2);
    x                 = x - idotn2 * nv.x;
    y                 = y - idotn2 * nv.y;
}
template <typename T>
void Vec2<typename T>::scaleVecTo(const Vec2& va, T scale)
{
    x = va.x * scale;
    y = va.y * scale;
}
template <typename T>
void Vec2<T>::subVecsTo(const Vec2& va, const Vec2& vb)
{
    x = va.x - vb.x;
    y = va.y - vb.y;
}
template <typename T>
void Vec2<T>::addVecsTo(const Vec2& va, const Vec2& vb)
{
    x = va.x + vb.x;
    y = va.y + vb.y;
}
template <typename T>
Vec2<T> Vec2<T>::subtract(const Vec2& v2) const
{
    return Vec2<T>(x - v2.x, y - v2.y);
}
template <typename T>
T Vec2<T>::crossBy(const Vec2& v2) const
{
    return x * v2.y - y * v2.x;
}
template <typename T>
Vec2<T> Vec2<T>::clone() const
{
    return Vec2<T>(x, y, z);
}


template <typename T>
void Vec2<T>::toArray2(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0]   = x;
        ls[1]   = y;
    }
}
template <typename T>
void Vec2<T>::toArray3(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0] = x;
        ls[1] = y;
        ls[2] = z;
    }
}

template <typename T>
Vec2<T>* Vec2<typename T>::fromArray2(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x = ls[0];
        y = ls[1];
    }
    return this;
}

template <typename T>
Vec2<T>* Vec2<typename T>::fromArray3(T* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x       = ls[0];
        y       = ls[1];
        z       = ls[2];
    }
    return this;
}


template <typename T>
std::string Vec2<T>::toString() const
{
    return "Vec2(x=" + std::to_string(x) + ",y=" + std::to_string(y) + ",z=" + std::to_string(z) + ")";
}

template class Vec2<double>;
template class Vec2<float>;
template class Vec2<char>;
template class Vec2<short>;
template class Vec2<int>;
template class Vec2<long>;

} // namespace math
} // namespace voxengine