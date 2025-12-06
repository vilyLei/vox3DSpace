#ifndef VOXOL_TILE_GRID_DEF_H
#define VOXOL_TILE_GRID_DEF_H


#include <cmath>
#include <cstdint>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "../Math/Vec2.h"
#include "../Render/OglGpuResUtils.h"
#include <format>

namespace Voxol::Tile
{

namespace RC
{

struct Pos
{
    union
    {
        int64_t value;

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

    constexpr Pos() noexcept :
        value(0)
        {};
    constexpr Pos(int32_t r_, int32_t c_, int32_t level_ = 0) noexcept :
        r(r_), c(c_), level(level_) {}

    bool operator==(const Pos& other) const noexcept
    {
        return value == other.value;
    }
    bool operator!=(const Pos& other) const noexcept
    {
        return value != other.value;
    }
    std::string toStringWithRC()
    {
        return std::format("RC::Pos(r={}, c={}, level={})\n", static_cast<int>(r), static_cast<int>(c), static_cast<int>(level));
    }
    std::string toStringWithXY()
    {
        return std::format("RC::Pos(x={}, y={}, depth={})\n", static_cast<int>(x), static_cast<int>(y), static_cast<int>(depth));
    }
};

union Rect
{
    int32_t data[4];

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

    constexpr Rect() noexcept :
        data{}
        {};
    constexpr Rect(int32_t minC_, int32_t minR_, int32_t maxC_, int32_t maxR_) noexcept :
        minC(minC_), minR(minR_), maxC(maxC_), maxR(maxR_)
    {}

    [[nodiscard]] int32_t width() const noexcept;
    [[nodiscard]] int32_t height() const noexcept;

    [[nodiscard]] bool contains(int32_t r, int32_t c) const noexcept;
    [[nodiscard]] bool contains(const Pos& pos) const noexcept;

    [[nodiscard]] bool overlaps(const Rect& other) const noexcept;

    void expand(int32_t margin) noexcept;

    [[nodiscard]] Rect intersectWith(const Rect& other) const noexcept;

    [[nodiscard]] Rect unionWith(const Rect& other) const noexcept;

    [[nodiscard]] bool isEqual(const Rect& other) const noexcept {
        return other.minX == minX && other.minY == minY && other.maxX == maxX && other.maxY == maxY;
    }
    [[nodiscard]] bool isNotEqual(const Rect& other) const noexcept {
        return other.minX != minX || other.minY != minY || other.maxX != maxX || other.maxY != maxY;
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
    Render::Gpu::DrawingUnit drawUnit{};
    RC::Pos                rc{};
    float areaSize = 256;
    void setRCAndAreaSize(const RC::Pos& pos, float pareaSize);
};
struct IndexNode
{
    RC::Pos pos{};
    int32_t index = -1;
    int     phase = 0;
    bool    dirty = false;
    bool    empty = false;
};
}
} // namespace Voxol::Tile
#endif