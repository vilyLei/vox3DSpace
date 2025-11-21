
#include "Vec3.h"

namespace Voxol::Math
{

bool Vec3::isEqual(const Vec3& pos) const {
    return std::abs(pos.x - x) < 1e-5f && std::abs(pos.y - y) < 1e-5f && std::abs(pos.z - z) < 1e-5f;
}

} // namespace Voxol::Math