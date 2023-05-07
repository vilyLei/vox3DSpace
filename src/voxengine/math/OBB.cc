#include <iostream>
#include <cmath>
#include "mathDefine.h"
#include "OBB.h"

namespace voxengine
{
namespace math
{

template <typename NumberType>
NumberType OBB<NumberType>::AbsR[3][3]{0.0f};
template <typename NumberType>
NumberType OBB<NumberType>::R[3][3]{0.0f};

template <typename NumberType>
OBB<typename NumberType>::OBB() noexcept
    :
    version(-1),
    radius(0.0f),
    m_ts{0.0f},
    extents{0.0}
{
}

template <typename NumberType>
void OBB<NumberType>::update()
{
    version++;
    for (auto i = 0; i < 3; ++i) axes[i].normalize();
    auto et      = extent;
    radius = et.getLength();
    //et.toArray3(extents.data());
    et.toArray3(extents);
}
template <typename NumberType>
bool OBB<NumberType>::containsV(const Vec3<NumberType>& pv) noexcept
{
    m_pv.subVecsTo(pv, center);
    auto v0 = m_pv;

    using namespace std;

    auto& et = extent;
    return abs(v0.dot(axes[0])) <= et.x && abs(v0.dot(axes[1])) <= et.y && abs(v0.dot(axes[2])) <= et.z;
}
template <typename NumberType>
bool OBB<NumberType>::intersect(const OBB& obb, NumberType epsilon)
{
    auto& a = *this;
    auto& b = obb;

    using namespace std;

    // 计算距离向量tv
    m_pv.subVecsTo(b.center, a.center);
    auto& tv = m_pv;

    if (tv.getLength() - (a.radius + b.radius) > epsilon)
    {
        return false;
    }
    auto Avs = OBB<NumberType>::AbsR;
    auto Rvs = OBB<NumberType>::R;

    // 计算旋转矩阵R
    for (auto i = 0; i < 3; ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            Rvs[i][j] = a.axes[i].dot(b.axes[j]);
        }
    }

    // 应用距离向量tv
    auto ts = m_ts;
    ts[0]   = tv.dot(a.axes[0]);
    ts[1]   = tv.dot(a.axes[1]);
    ts[2]   = tv.dot(a.axes[2]);

    // 计算旋转矩阵R的绝对值AbsR
    for (auto i = 0; i < 3; ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            Avs[i][j] = abs(Rvs[i][j]) + epsilon;
        }
    }

    auto& aets = a.extents;
    auto& bets = b.extents;

    NumberType ra = 0.0f;
    NumberType rb = 0.0f;

    // test axes(A0, A1, A2)
    for (auto i = 0; i < 3; ++i)
    {
        ra = aets[i];
        rb = bets[0] * Avs[i][0] + bets[1] * Avs[i][1] + bets[2] * Avs[i][2];
        if (abs(ts[i]) > ra + rb) return false;
    }

    // test axes(B0, B1, B2)
    for (auto i = 0; i < 3; ++i)
    {
        ra = aets[0] * Avs[0][i] + aets[1] * Avs[1][i] + aets[2] * Avs[2][i];
        rb = bets[i];
        if (abs(ts[0] * Rvs[0][i] + ts[1] * Rvs[1][i] + ts[2] * Rvs[2][i]) > ra + rb) return false;
    }

    // test axes L = A0 x B0
    ra = aets[1] * Avs[2][0] + aets[2] * Avs[1][0];
    rb = bets[1] * Avs[0][2] + bets[2] * Avs[0][1];
    if (abs(ts[2] * Rvs[1][0] - ts[1] * Rvs[2][0]) > ra + rb) return false;

    // test axes L = A0 x B1
    ra = aets[1] * Avs[2][1] + aets[2] * Avs[1][1];
    rb = bets[0] * Avs[0][2] + bets[2] * Avs[0][0];
    if (abs(ts[2] * Rvs[1][1] - ts[1] * Rvs[2][1]) > ra + rb) return false;

    // test axes L = A0 x B2
    ra = aets[1] * Avs[2][2] + aets[2] * Avs[1][2];
    rb = bets[0] * Avs[0][1] + bets[1] * Avs[0][0];
    if (abs(ts[2] * Rvs[1][2] - ts[1] * Rvs[2][2]) > ra + rb) return false;

    // --------------------------------------------------------------------------

    // test axes L = A1 x B0
    ra = aets[0] * Avs[2][0] + aets[2] * Avs[0][0];
    rb = bets[1] * Avs[1][2] + bets[2] * Avs[1][1];
    if (abs(ts[0] * Rvs[2][0] - ts[2] * Rvs[0][0]) > ra + rb) return false;

    // test axes L = A1 x B1
    ra = aets[0] * Avs[2][1] + aets[2] * Avs[0][1];
    rb = bets[0] * Avs[1][2] + bets[2] * Avs[1][0];
    if (abs(ts[0] * Rvs[2][1] - ts[2] * Rvs[0][1]) > ra + rb) return false;

    // test axes L = A1 x B2
    ra = aets[0] * Avs[2][2] + aets[2] * Avs[0][2];
    rb = bets[0] * Avs[1][1] + bets[1] * Avs[1][0];
    if (abs(ts[0] * Rvs[2][2] - ts[2] * Rvs[0][2]) > ra + rb) return false;

    // --------------------------------------------------------------------------

    // test axes L = A2 x B0
    ra = aets[0] * Avs[1][0] + aets[1] * Avs[0][0];
    rb = bets[1] * Avs[2][2] + bets[2] * Avs[2][1];
    if (abs(ts[1] * Rvs[0][0] - ts[0] * Rvs[1][0]) > ra + rb) return false;

    // test axes L = A2 x B1
    ra = aets[0] * Avs[1][1] + aets[1] * Avs[0][1];
    rb = bets[0] * Avs[2][2] + bets[2] * Avs[2][0];
    if (abs(ts[1] * Rvs[0][1] - ts[0] * Rvs[1][1]) > ra + rb) return false;

    // test axes L = A2 x B2
    ra = aets[0] * Avs[1][2] + aets[1] * Avs[0][2];
    rb = bets[0] * Avs[2][1] + bets[1] * Avs[2][0];
    if (abs(ts[1] * Rvs[0][2] - ts[0] * Rvs[1][2]) > ra + rb) return false;

    return true;
}

template <typename NumberType>
void __$templateConstructOBB(NumberType value)
{
    OBB<NumberType> va{};
    OBB<NumberType> v{};

    v.containsV(va.center);
    v.intersect(va);
    v.update();
}
void __$templateImplyOBB()
{
    __$templateConstructOBB(1.0);
    __$templateConstructOBB(1.0f);
}
} // namespace math
} // namespace voxengine