#ifndef VOXOL_TILE_GRID_DEF_H
#define VOXOL_TILE_GRID_DEF_H


#include <cmath>
#include <cstdint>
#include "../Base/BaseDefine.h"
#include "../Math/VxRect.h"
#include "../Math/Vec2.h"
#include "../Test/OglResUtils.h""

namespace Voxol::Tile
{
using namespace Voxol::Math;

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

inline Pos          xyToRC(float x, float y, float areaSize, int32_t depth = 0);
inline Math::Vec2   rcToXY(const Pos& rc, float areaSize);
inline Rect         fromWorldBounds(const Math::VxRect& bounds, int32_t areaSize, float offset = 0.0f);
inline Math::VxRect toWorldBounds(const Rect& rc, int32_t areaSize);

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
}
} // namespace Voxol::Tile
#endif