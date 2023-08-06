#include <iostream>
#include <cmath>
#include "mathDefine.h"
#include "Vec2.h"

namespace voxengine
{
namespace math
{

template <typename NumberType>
const NumberType Vec2<NumberType>::s_180OverPi = static_cast<NumberType>(MATH_180_OVER_PI);
template <typename NumberType>
const NumberType Vec2<typename NumberType>::s_minv = getPositiveMinValue<NumberType>();


//template <typename NumberType>
//const Vec2<NumberType> Vec2<NumberType>::X_AXIS{static_cast<NumberType>(1), static_cast<NumberType>(0), static_cast<NumberType>(0)};
//template <typename NumberType>
//const Vec2<NumberType> Vec2<NumberType>::Y_AXIS{static_cast<NumberType>(0), static_cast<NumberType>(1), static_cast<NumberType>(0)};
//template <typename NumberType>
//const Vec2<NumberType> Vec2<NumberType>::ZERO{static_cast<NumberType>(0), static_cast<NumberType>(0), static_cast<NumberType>(0)};
//template <typename NumberType>
//const Vec2<NumberType> Vec2<NumberType>::ONE{static_cast<NumberType>(1), static_cast<NumberType>(1), static_cast<NumberType>(1)};
CREATE_SATATIC_VEC2_INS(NumberType, X_AXIS, 1, 0, 0);
CREATE_SATATIC_VEC2_INS(NumberType, Y_AXIS, 0, 1, 0);
CREATE_SATATIC_VEC2_INS(NumberType, ZERO, 0, 0, 0);
CREATE_SATATIC_VEC2_INS(NumberType, ONE, 1, 1, 1);

template <typename NumberType>
Vec2<NumberType> Vec2<NumberType>::s_v0{};
template <typename NumberType>
Vec2<NumberType> Vec2<NumberType>::s_v1{};


template <typename NumberType>
Vec2<NumberType>::Vec2(NumberType px, NumberType py, NumberType pw) noexcept
    :
    x(px), y(py), w(pw)
{
    //std::cout << "Vec2::constructor() ..." << std::endl;
}
template <typename NumberType>
Vec2<NumberType>::Vec2() noexcept
    :
    x(static_cast<NumberType>(0)), y(static_cast<NumberType>(0)), w(static_cast<NumberType>(1))
{
    //std::cout << "Vec2::constructor() default..." << std::endl;
    //std::cout << std::boolalpha
    //std::cout << "sizeof(NumberType): " << sizeof(NumberType) << ", s_minv: " << s_minv << std::endl;
    //std::cout << "0.0000001 > s_minv: " << (0.0000001 > s_minv) << std::endl;
}

template <typename NumberType>
NumberType& Vec2<NumberType>::operator[](unsigned int i)
{

    switch (i)
    {
        case 1:
            return y;
            break;
        case 2:
            return w;
            break;
        default:
            break;
    }
    return x;
}

template <typename NumberType>
void Vec2<NumberType>::set(NumberType px, NumberType py, NumberType pw)
{
    x = px;
    y = py;
    w = pw;
}

template <typename NumberType>
void Vec2<NumberType>::setXY(NumberType px, NumberType py)
{
    x = px;
    y = py;
}

template <typename NumberType>
NumberType Vec2<NumberType>::dot(const Vec2& v2) const
{
    return x * v2.x + y * v2.y;
}
template <typename NumberType>
NumberType Vec2<NumberType>::getLength() const
{
    return std::sqrt(x * x + y * y);
}
template <typename NumberType>
NumberType Vec2<NumberType>::getLengthSquared() const
{
    return x * x + y * y;
}

template <typename NumberType>
void Vec2<NumberType>::copyFrom(const Vec2& v2)
{
    x = v2.x;
    y = v2.y;
}
template <typename NumberType>
void Vec2<NumberType>::multBy(const Vec2& v2)
{
    x *= v2.x;
    y *= v2.y;
}
template <typename NumberType>
void Vec2<NumberType>::normalize()
{
    NumberType d = sqrt(x * x + y * y);
    if (d > s_minv)
    {
        x /= d;
        y /= d;
    }
}
template <typename NumberType>
void Vec2<NumberType>::normalizeTo(Vec2& v2) const
{
    auto d = static_cast<NumberType>( std::sqrt(x * x + y * y) );
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
template <typename NumberType>
void Vec2<NumberType>::scaleVector(const Vec2& v2)
{
    x *= v2.x;
    y *= v2.y;
}
template <typename NumberType>
void Vec2<NumberType>::scaleBy(NumberType s)
{
    x *= s;
    y *= s;
}
template <typename NumberType>
void Vec2<NumberType>::negate()
{
    x = -x;
    y = -y;
}
template <typename NumberType>
bool Vec2<NumberType>::equalsXYZ(const Vec2& v2)
{
    return std::abs(x - v2.x) < s_minv && std::abs(y - v2.y) < s_minv;
}
template <typename NumberType>
bool Vec2<NumberType>::equalsAll(const Vec2& v2)
{
    return std::abs(x - v2.x) < s_minv && std::abs(y - v2.y) < s_minv && std::abs(w - v2.w) < s_minv;
}
template <typename NumberType>
void Vec2<NumberType>::project()
{
    NumberType t = static_cast<NumberType>(1) / w;
    x *= t;
    y *= t;
}
template <typename NumberType>
void Vec2<NumberType>::addBy(const Vec2& v2)
{
    x += v2.x;
    y += v2.y;
}
template <typename NumberType>
void Vec2<NumberType>::subtractBy(const Vec2& v2)
{
    x -= v2.x;
    y -= v2.y;
}
template <typename NumberType>
void Vec2<NumberType>::reflectBy(const Vec2& nv)
{
    NumberType idotn2 = (x * nv.x + y * nv.y) * static_cast<NumberType>(2);
    x                 = x - idotn2 * nv.x;
    y                 = y - idotn2 * nv.y;
}
template <typename NumberType>
void Vec2<typename NumberType>::scaleVecTo(const Vec2& va, NumberType scale)
{
    x = va.x * scale;
    y = va.y * scale;
}
template <typename NumberType>
void Vec2<NumberType>::subVecsTo(const Vec2& va, const Vec2& vb)
{
    x = va.x - vb.x;
    y = va.y - vb.y;
}
template <typename NumberType>
void Vec2<NumberType>::addVecsTo(const Vec2& va, const Vec2& vb)
{
    x = va.x + vb.x;
    y = va.y + vb.y;
}
template <typename NumberType>
Vec2<NumberType> Vec2<NumberType>::subtract(const Vec2& v2) const
{
    return Vec2<NumberType>(x - v2.x, y - v2.y);
}
template <typename NumberType>
NumberType Vec2<NumberType>::crossBy(const Vec2& v2) const
{
    return x * v2.y - y * v2.x;
}
template <typename NumberType>
Vec2<NumberType> Vec2<NumberType>::clone() const
{
    return Vec2<NumberType>(x, y, w);
}


template <typename NumberType>
void Vec2<NumberType>::toArray2(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0]   = x;
        ls[1]   = y;
    }
}
template <typename NumberType>
void Vec2<NumberType>::toArray3(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        ls[0] = x;
        ls[1] = y;
        ls[2] = w;
    }
}

template <typename NumberType>
Vec2<NumberType>* Vec2<typename NumberType>::fromArray2(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x = ls[0];
        y = ls[1];
    }
    return this;
}

template <typename NumberType>
Vec2<NumberType>* Vec2<typename NumberType>::fromArray3(NumberType* arr, unsigned int offset)
{
    if (arr != nullptr)
    {
        auto ls = arr + offset;
        x       = ls[0];
        y       = ls[1];
        w       = ls[2];
    }
    return this;
}


template <typename NumberType>
std::string Vec2<NumberType>::toString()
{
    return "Vec2(x=" + std::to_string(x) + ",y=" + std::to_string(y) + ",w=" + std::to_string(w) + ")";
}

template class Vec2<double>;
template class Vec2<float>;
template class Vec2<char>;
template class Vec2<short>;
template class Vec2<int>;
template class Vec2<long>;

} // namespace math
} // namespace voxengine