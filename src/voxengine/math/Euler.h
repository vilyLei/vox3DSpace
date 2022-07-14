#pragma once
#include "IEuler.h"
namespace voxengine
{
namespace math
{
template <typename NumberType>
class Euler : public IEuler<NumberType>
{
public:
    Euler() noexcept;
    ~Euler() = default;

    NumberType x;
    NumberType y;
    NumberType z;
    EulerOrder order;
    void set(NumberType px, NumberType py, NumberType pz, EulerOrder porder = EulerOrder::XYZ);

private:
};

}
} // namespace voxengine