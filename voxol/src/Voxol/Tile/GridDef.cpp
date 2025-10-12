#include "GridDef.h"

namespace Voxol::Tile
{
namespace RC
{

Rect::Rect(int32_t minC, int32_t minR, int32_t maxC, int32_t maxR) :
    minC(minC), minR(minR), maxC(maxC), maxR(maxR) {}

[[nodiscard]] int32_t Rect::width() const noexcept { return maxC - minC + 1; }
[[nodiscard]] int32_t Rect::height() const noexcept { return maxR - minR + 1; }

[[nodiscard]] bool Rect::contains(int32_t r, int32_t c) const noexcept
{
    return (r >= minR && r <= maxR && c >= minC && c <= maxC);
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


//inline Pos xyToRC(float x, float y, float areaSize, int32_t depth)
//{
//    auto r = static_cast<int32_t>(std::floor(static_cast<double>(y) / areaSize));
//    auto c = static_cast<int32_t>(std::floor(static_cast<double>(x) / areaSize));
//    return {r, c, depth};
//}
//inline Math::Vec2 rcToXY(const Pos& rc, float areaSize)
//{
//    return {rc.c * areaSize, rc.r * areaSize};
//}
//inline Rect xyRectToRCRect(const Math::VxRect& bounds, float areaSize, float offset)
//{
//    Rect rect;
//    rect.minR = static_cast<int32_t>(std::floor((bounds.y() - offset) / areaSize));
//    rect.minC = static_cast<int32_t>(std::floor((bounds.x() - offset) / areaSize));
//    rect.maxR = static_cast<int32_t>(std::floor((bounds.bottom() + offset) / areaSize));
//    rect.maxC = static_cast<int32_t>(std::floor((bounds.right() + offset) / areaSize));
//    return rect;
//}
//inline Math::VxRect rcRectToXYRect(const Rect& rc, float areaSize)
//{
//    Math::VxRect b{};
//    b.fX  = rc.minC * areaSize;
//    b.fY  = rc.minR * areaSize;
//    b.fRX = (rc.maxC + 1) * areaSize;
//    b.fBY = (rc.maxR + 1) * areaSize;
//    return b;
//}

} // namespace RC

namespace Grid
{

void Unit::setRCAndAreaSize(const RC::Pos& pos, float pareaSize) {
    rc        = pos;
    areaSize  = pareaSize;
    auto&& xy = RC::rcToXY(pos, areaSize);
    drawUnit.objMat.setTo(xy.x, xy.y, areaSize, areaSize);
}

}
} // namespace Voxol::Tile