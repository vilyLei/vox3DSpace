#include "Euler.h"
#include "Matrix4.h"
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
Euler<NumberType>* Euler<NumberType>::setFromRotationMatrix(const IMatrix4<NumberType>& m, EulerOrder order)
{
    return this;
}

template <typename NumberType>
void __$templateConstructEuler(NumberType value)
{

    Euler<NumberType> eu0;
    Matrix4<NumberType> mat0;
    eu0.set(value, value, value);
    eu0.setFromRotationMatrix(mat0);
}
void __$templateImplyEuler()
{
    __$templateConstructEuler(1.0);
    __$templateConstructEuler(1.0f);
}

} // namespace math
} // namespace voxengine
