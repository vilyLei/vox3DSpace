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

    PositionKey(int32_t x, int32_t y, int16_t d)
    {
        depth = d;
        x   = r;
        y   = c;
    }
};

} // namespace Voxol::Base
#endif
