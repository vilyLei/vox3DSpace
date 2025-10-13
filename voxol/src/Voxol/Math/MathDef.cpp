#include "MathDef.h"
namespace Voxol::Math
{
double calcCeilOfTwoLevel(double value)
{
    return std::ceil(std::log(value) / MATH_LN2);
}

float calcCeilOfTwoLevel(float value)
{
    return std::ceil(std::log(value) / MATH_LN2);
}

int calcCeilOfTwoLevelToInt(float value)
{
    return static_cast<int>(std::ceil(std::log(value) / MATH_LN2));
}
} // namespace Voxol::Math