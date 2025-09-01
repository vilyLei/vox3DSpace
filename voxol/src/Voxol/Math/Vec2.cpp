
#include "Vec2.h"

namespace Voxol::Math
{
    
Vec2 Vec2::operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
Vec2 Vec2::operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
Vec2 Vec2::operator*(float s) const { return {x * s, y * s}; }

} // namespace Voxol::Math