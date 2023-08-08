#include "Euler.h"
#include "Matrix4.h"
namespace voxengine
{
namespace math
{
template <typename T>
Euler<T>::Euler() noexcept
    :
    x(0.0f), y(0.0f), z(0.0f), order(EulerOrder::XYZ)
{
}

template <typename T>
void Euler<T>::set(T px, T py, T pz, EulerOrder porder)
{

}

template <typename T>
Euler<T>* Euler<T>::setFromRotationMatrix(const IMatrix4<T>& m, EulerOrder order)
{
    return this;
}
template class Euler<double>;
template class Euler<float>;

} // namespace math
} // namespace voxengine
