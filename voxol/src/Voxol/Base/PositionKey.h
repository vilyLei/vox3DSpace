#ifndef VOXOL_BASE_POSITION_KEY_H
#define VOXOL_BASE_POSITION_KEY_H

#include <cstdint>

namespace Voxol::Base
{
struct PositionKey
{
    union
    {
        uint64_t value;
        struct
        {
            uint64_t level : 16;
            uint64_t r : 24;
            uint64_t c : 24;
        };

        struct
        {
            uint64_t depth : 16;
            uint64_t y : 24;
            uint64_t x : 24;
        };
    };

    PositionKey(uint32_t x, uint32_t y, uint16_t d)
    {
        depth = d;
        x   = r;
        y   = c;
    }
};

} // namespace Voxol::Base
#endif
