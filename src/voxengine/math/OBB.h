#pragma once
#include "Vec3.h"
#include <array>

namespace voxengine
{
namespace math
{
template <typename NumberType>
class OBB
{
public:
    OBB()
    noexcept;
    ~OBB() = default;

    /**
     * three axes normalization 3d vectors
     */
    std::array<Vec3<NumberType>, 3> axes;
    std::array<NumberType, 3>             extents;
    /**
     * half length of these three axes
     */
    Vec3<NumberType> extent;
    Vec3<NumberType> center;
    int              version;
    NumberType       radius;
    void             update();
    bool             containsV(const Vec3<NumberType>& pv) noexcept;
    bool             intersect(const OBB& a, NumberType epsilon = 1e-6f);

private:
    Vec3<NumberType>  m_pv;
    NumberType        m_ts[3];
    static NumberType AbsR[3][3];
    static NumberType R[3][3];
};
} // namespace math
} // namespace voxengine