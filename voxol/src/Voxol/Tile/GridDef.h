#ifndef VOXOL_TILE_GRID_DEF_H
#define VOXOL_TILE_GRID_DEF_H


#include <cmath>
#include <cstdint>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"

namespace Voxol::Tile
{
using namespace Voxol::Math;

namespace RC
{

union GridRect
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

    GridRect() = default;
    GridRect(int32_t minC, int32_t minR, int32_t maxC, int32_t maxR);

    [[nodiscard]] int32_t width() const noexcept;
    [[nodiscard]] int32_t height() const noexcept;

    [[nodiscard]] bool contains(int32_t r, int32_t c) const noexcept;

    [[nodiscard]] bool overlaps(const GridRect& other) const noexcept;

    void expand(int32_t margin) noexcept;

    [[nodiscard]] GridRect intersectWith(const GridRect& other) const noexcept;

    [[nodiscard]] GridRect unionWith(const GridRect& other) const noexcept;
};

struct RectPos
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

    RectPos() = default;
    RectPos(int32_t r, int32_t c, int32_t level = 0) :
        r(r), c(c), level(level) {}

    [[nodiscard]] bool operator==(const RectPos& other) const noexcept
    {
        return value == other.value;
    }
    [[nodiscard]] bool operator!=(const RectPos& other) const noexcept
    {
        return value != other.value;
    }
};

inline RectPos      xyToRC(int32_t x, int32_t y, int32_t areaSize);
inline RectPos      rcToXY(const RectPos& rc, int32_t areaSize);
inline GridRect     fromWorldBounds(const Math::VxRect& bounds, int32_t areaSize, float offset = 0.0f);
inline Math::VxRect toWorldBounds(const GridRect& rc, int32_t areaSize);

} // namespace RC

} // namespace Voxol::Tile
#endif