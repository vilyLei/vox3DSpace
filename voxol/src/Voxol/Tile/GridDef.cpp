#include "GridDef.h"

namespace Voxol::Tile
{
using namespace Voxol::Math;

namespace RC
{

GridRect::GridRect(int32_t minC, int32_t minR, int32_t maxC, int32_t maxR) :
    minC(minC), minR(minR), maxC(maxC), maxR(maxR) {}

[[nodiscard]] int32_t GridRect::width() const noexcept { return maxC - minC + 1; }
[[nodiscard]] int32_t GridRect::height() const noexcept { return maxR - minR + 1; }

[[nodiscard]] bool GridRect::contains(int32_t r, int32_t c) const noexcept
{
    return (r >= minR && r <= maxR && c >= minC && c <= maxC);
}

[[nodiscard]] bool GridRect::overlaps(const GridRect& other) const noexcept
{
    return !(other.minC > maxC || other.maxC < minC ||
             other.minR > maxR || other.maxR < minR);
}

void GridRect::expand(int32_t margin) noexcept
{
    minC -= margin;
    minR -= margin;
    maxC += margin;
    maxR += margin;
}

[[nodiscard]] GridRect GridRect::intersectWith(const GridRect& other) const noexcept
{
    GridRect r;
    r.minC = std::max(minC, other.minC);
    r.minR = std::max(minR, other.minR);
    r.maxC = std::min(maxC, other.maxC);
    r.maxR = std::min(maxR, other.maxR);
    return r;
}

[[nodiscard]] GridRect GridRect::unionWith(const GridRect& other) const noexcept
{
    GridRect r;
    r.minC = std::min(minC, other.minC);
    r.minR = std::min(minR, other.minR);
    r.maxC = std::max(maxC, other.maxC);
    r.maxR = std::max(maxR, other.maxR);
    return r;
}


inline RectPos xyToRC(float x, float y, float areaSize)
{
    auto r = static_cast<int32_t>(std::floor(static_cast<double>(y) / areaSize));
    auto c = static_cast<int32_t>(std::floor(static_cast<double>(x) / areaSize));
    return {r, c, 0};
}

inline Math::Vec2 rcToXY(const RectPos& rc, float areaSize)
{
    Math::Vec2 pos;
    pos.x     = rc.c * areaSize;
    pos.y     = rc.r * areaSize;
    return pos;
}

inline GridRect fromWorldBounds(const Math::VxRect& bounds, int32_t areaSize, float offset)
{
    GridRect rect;
    rect.minR = static_cast<int32_t>(std::floor((bounds.y() - offset) / areaSize));
    rect.minC = static_cast<int32_t>(std::floor((bounds.x() - offset) / areaSize));
    rect.maxR = static_cast<int32_t>(std::floor((bounds.bottom() + offset) / areaSize));
    rect.maxC = static_cast<int32_t>(std::floor((bounds.right() + offset) / areaSize));
    return rect;
}

inline Math::VxRect toWorldBounds(const GridRect& rc, int32_t areaSize)
{
    Math::VxRect b{};
    b.fX  = rc.minC * areaSize;
    b.fY  = rc.minR * areaSize;
    b.fRX = (rc.maxC + 1) * areaSize;
    b.fBY = (rc.maxR + 1) * areaSize;
    return b;
}

} // namespace RC
} // namespace Voxol::Tile