#include "GridDef.h"

namespace Voxol::Tile
{
namespace RC
{

[[nodiscard]] int32_t Rect::width() const noexcept { return maxC - minC + 1; }
[[nodiscard]] int32_t Rect::height() const noexcept { return maxR - minR + 1; }

[[nodiscard]] bool Rect::contains(int32_t r, int32_t c) const noexcept
{
    return (r >= minR && r <= maxR && c >= minC && c <= maxC);
}
[[nodiscard]] bool Rect::contains(const Pos& pos) const noexcept
{
    return (pos.r >= minR && pos.r <= maxR && pos.c >= minC && pos.c <= maxC);
}

[[nodiscard]] bool Rect::overlaps(const Rect& other) const noexcept
{
    return !(other.minC > maxC || other.maxC < minC ||
             other.minR > maxR || other.maxR < minR);
}

void Rect::expand(int32_t margin) noexcept
{
    minC -= margin;
    minR -= margin;
    maxC += margin;
    maxR += margin;
}

[[nodiscard]] Rect Rect::intersectWith(const Rect& other) const noexcept
{
    Rect r;
    r.minC = std::max(minC, other.minC);
    r.minR = std::max(minR, other.minR);
    r.maxC = std::min(maxC, other.maxC);
    r.maxR = std::min(maxR, other.maxR);
    return r;
}

[[nodiscard]] Rect Rect::unionWith(const Rect& other) const noexcept
{
    Rect r;
    r.minC = std::min(minC, other.minC);
    r.minR = std::min(minR, other.minR);
    r.maxC = std::max(maxC, other.maxC);
    r.maxR = std::max(maxR, other.maxR);
    return r;
}


} // namespace RC

namespace Grid
{

void Unit::setRCAndAreaSize(const RC::Pos& pos, float pareaSize) {
    rc        = pos;
    areaSize  = pareaSize;
    auto&& xy = RC::rcToXY(pos, areaSize);
    drawUnit.setTranslateAndScale(xy.x, xy.y, areaSize, areaSize);
}

}
} // namespace Voxol::Tile