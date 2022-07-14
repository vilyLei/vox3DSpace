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
} // namespace math
} // namespace voxengine
