#pragma once
namespace voxengine
{
namespace math
{
#define MATH_E             2.71828182845904523536  // e
#define MATH_LOG2E         1.44269504088896340736  // log2(e)
#define MATH_LOG10E        0.43429448190325182765  // log10(e)
#define MATH_LN2           0.69314718055994530941  // ln(2)
#define MATH_LN10          2.30258509299404568402  // ln(10)
#define MATH_PI            3.14159265358979323846  // pi
#define MATH_2PI           6.28318530717958647652  // 2pi
#define MATH_PI_OVER_2     1.57079632679489661923  // pi/2
#define MATH_PI_OVER_4     0.78539816339744830962  // pi/4
#define MATH_1_OVER_PI     0.31830988618379067154  // 1/pi
#define MATH_2_OVER_PI     0.63661977236758134308  // 2/pi
#define MATH_180_OVER_PI   57.29577951308232286465 // 180/pi
#define MATH_PI_OVER_180   0.01745329251994329547  // pi/180
#define MATH_2_OVER_SQRTPI 1.12837916709551257390  // 2/sqrt(pi)
#define MATH_SQRT2         1.41421356237309504880  // sqrt(2)
#define MATH_1_OVER_SQRT2  0.70710678118654752440  // 1/sqrt(2)

template <typename T>
constexpr T getPositiveMinValue()
{
    if constexpr ((std::is_arithmetic<T>::value) && (std::is_floating_point<T>::value))
    {
        return sizeof(T) > 4 ? 1e-9 : 1e-7f;
    }
    else {
        return 0;
    }
}
template <typename T>
T degreeToRadian(T degree)
{
    return MATH_PI_OVER_180 * degree;
}
} // namespace math
} // namespace voxengine