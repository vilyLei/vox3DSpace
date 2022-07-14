#include "Euler.h"
namespace voxengine
{
namespace math
{
template <typename NumberType>
Euler<NumberType>::Euler() noexcept
    :
    x(0.0f), y(0.0f), z(0.0f), order(EulerOrder::XYZ)
{
}

template <typename NumberType>
void Euler<NumberType>::set(NumberType px, NumberType py, NumberType pz, EulerOrder porder)
{

}


template <typename NumberType>
void __$templateConstructEuler(NumberType value)
{

    Euler<NumberType> eu0;
    eu0.set(value, value, value);
}
void __$templateImplyEuler()
{
    __$templateConstructEuler(1.0);
    __$templateConstructEuler(1.0f);
}

} // namespace math
} // namespace voxengine
