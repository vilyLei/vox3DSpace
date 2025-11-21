
#include "Vec2.h"

namespace Voxol::Math
{

bool Vec2::isEqual(const Vec2& pos) const {
    return std::abs(pos.x - x) < 1e-5f && std::abs(pos.y - y) < 1e-5f;
}

} // namespace Voxol::Math