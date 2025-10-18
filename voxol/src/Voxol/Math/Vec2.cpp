
#include "Vec2.h"

namespace Voxol::Math
{
    
//Vec2 Vec2::operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
//Vec2 Vec2::operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
//Vec2 Vec2::operator*(float s) const { return {x * s, y * s}; }

bool Vec2::isEqual(const Vec2& pos) const {
    return std::abs(pos.x - x) < 1e-5f && std::abs(pos.y - y) < 1e-5f;
}

} // namespace Voxol::Math