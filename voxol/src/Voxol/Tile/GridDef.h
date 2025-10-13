#ifndef VOXOL_TILE_GRID_DEF_H
#define VOXOL_TILE_GRID_DEF_H


#include <cmath>
#include <cstdint>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "../Math/Vec2.h"
#include "../Test/OglResUtils.h"

namespace Voxol::Tile
{

namespace RC
{

union Rect
{
    int32_t data[4]{};

    struct
    {
        int32_t minC;
        int32_t minR;
        int32_t maxC;
        int32_t maxR;
    };

    struct
    {
        int32_t minX;
        int32_t minY;
        int32_t maxX;
        int32_t maxY;
    };

    Rect() = default;
    Rect(int32_t minC, int32_t minR, int32_t maxC, int32_t maxR);

    [[nodiscard]] int32_t width() const noexcept;
    [[nodiscard]] int32_t height() const noexcept;

    [[nodiscard]] bool contains(int32_t r, int32_t c) const noexcept;

    [[nodiscard]] bool overlaps(const Rect& other) const noexcept;

    void expand(int32_t margin) noexcept;

    [[nodiscard]] Rect intersectWith(const Rect& other) const noexcept;

    [[nodiscard]] Rect unionWith(const Rect& other) const noexcept;

    [[nodiscard]] inline bool isEqual(const Rect& other) const noexcept {
        return other.minX == minX && other.minY == minY && other.maxX == maxX && other.minX == maxY;
    }
    [[nodiscard]] inline bool isNotEqual(const Rect& other) const noexcept {
        return other.minX != minX || other.minY != minY || other.maxX != maxX || other.minX != maxY;
    }
};

struct Pos
{
    union
    {
        int64_t value = 0;

        struct
        {
            int64_t level : 16;
            int64_t c : 24;
            int64_t r : 24;
        };

        struct
        {
            int64_t depth : 16;
            int64_t x : 24;
            int64_t y : 24;
        };
    };

    Pos() = default;
    Pos(int32_t r, int32_t c, int32_t level = 0) :
        r(r), c(c), level(level) {}

    [[nodiscard]] bool operator==(const Pos& other) const noexcept
    {
        return value == other.value;
    }
    [[nodiscard]] bool operator!=(const Pos& other) const noexcept
    {
        return value != other.value;
    }
};

inline Pos xyToRC(float x, float y, float areaSize, int32_t depth = 0)
{
    auto r = static_cast<int32_t>(std::floor(static_cast<double>(y) / areaSize));
    auto c = static_cast<int32_t>(std::floor(static_cast<double>(x) / areaSize));
    return {r, c, depth};
}
inline Math::Vec2 rcToXY(const Pos& rc, float areaSize)
{
    return {rc.c * areaSize, rc.r * areaSize};
}
inline Rect xyRectToRCRect(const Math::VxRect& bounds, float areaSize, float offset = 0)
{
    Rect rect;
    rect.minR = static_cast<int32_t>(std::floor((bounds.y() - offset) / areaSize));
    rect.minC = static_cast<int32_t>(std::floor((bounds.x() - offset) / areaSize));
    rect.maxR = static_cast<int32_t>(std::floor((bounds.bottom() + offset) / areaSize));
    rect.maxC = static_cast<int32_t>(std::floor((bounds.right() + offset) / areaSize));
    return rect;
}
inline Math::VxRect rcRectToXYRect(const Rect& rc, float areaSize)
{
    Math::VxRect b{};
    b.fX  = rc.minC * areaSize;
    b.fY  = rc.minR * areaSize;
    b.fRX = (rc.maxC + 1) * areaSize;
    b.fBY = (rc.maxR + 1) * areaSize;
    return b;
}

} // namespace RC

namespace Grid
{
struct Unit
{
    Test::Gpu::DrawingUnit drawUnit{};
    RC::Pos                rc{};
    float areaSize = 256;
    void setRCAndAreaSize(const RC::Pos& pos, float pareaSize);
};
struct IndexNode
{
    int64_t value = 0;
    int32_t index = 0;
};
}
} // namespace Voxol::Tile
#endif