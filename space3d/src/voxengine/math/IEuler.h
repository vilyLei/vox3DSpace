#pragma once
#include "EulerOrder.h"
namespace voxengine
{
namespace math
{

template<typename T>
class IEuler
{
public:
    T x;
    T y;
    T z;
    EulerOrder order;

    virtual void set(T px, T py, T pz, EulerOrder porder = EulerOrder::XYZ) = 0;
};

}
} // namespace voxengine