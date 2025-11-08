#ifndef VOXOL_BASE_POSITION_KEY_H
#define VOXOL_BASE_POSITION_KEY_H

#include <cstdint>

namespace Voxol::Base
{
struct PositionKey
{
    union
    {
        int64_t value;
        struct
        {
            int64_t level : 16;
            int64_t r : 24;
            int64_t c : 24;
        };

        struct
        {
            int64_t depth : 16;
            int64_t y : 24;
            int64_t x : 24;
        };
    };

    constexpr PositionKey() noexcept :
        value{} {}
    constexpr PositionKey(int32_t x_, int32_t y_, int32_t depth_) :
        depth(depth), x(x_), y(y_) {}

    static constexpr PositionKey makeWithXY(int32_t x_, int32_t y_, int32_t depth_)
    {
        return PositionKey{depth_, y_, x_};
    }
    static constexpr PositionKey makeWithRC(int32_t r_, int32_t c_, int32_t level_)
    {
        return PositionKey{level_, r_, c_};
    }
    constexpr bool operator==(const PositionKey& other) const noexcept
    {
        return value == other.value;
    }
    constexpr bool operator!=(const PositionKey& other) const noexcept
    {
        return value != other.value;
    }

    constexpr bool operator<(const PositionKey& other) const noexcept
    {
        return value < other.value;
    }
};

} // namespace Voxol::Base
#endif
