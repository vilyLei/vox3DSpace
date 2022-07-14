#pragma once
#include "EulerOrder.h"
namespace voxengine
{
namespace math
{

template<typename NumberType>
class IEuler
{
public:
    NumberType x;
    NumberType y;
    NumberType z;
    EulerOrder order;

    virtual void set(NumberType px, NumberType py, NumberType pz, EulerOrder porder = EulerOrder::XYZ) = 0;
};

}
} // namespace voxengine