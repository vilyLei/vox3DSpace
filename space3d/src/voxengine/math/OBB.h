#pragma once
#include "Vec3.h"

namespace voxengine
{
namespace math
{
template <typename T>
class OBB
{
public:
    OBB()
    noexcept;
    ~OBB() = default;

    /**
     * three axes normalization 3d vectors
     */
    Vec3<T> axes[3];
    T       extents[3];
    /**
     * half length of these three axes
     */
    Vec3<T> extent;
    Vec3<T> center;
    int              version;
    T       radius;
    void             update();
    bool             containsV(const Vec3<T>& pv) noexcept;
    bool             intersect(const OBB& a, T epsilon = 1e-6f);

private:
    Vec3<T>  m_pv;
    T        m_ts[3];
    static T AbsR[3][3];
    static T R[3][3];
};
} // namespace math
} // namespace voxengine