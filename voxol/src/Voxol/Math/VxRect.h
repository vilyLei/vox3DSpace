
#ifndef VOXOL_MATH_VX_RECT_H
#define VOXOL_MATH_VX_RECT_H

#include "Vec2.h"
#include <cmath>

namespace Voxol::Math
{

struct VxRect
{
public:
    union
    {
        struct
        {
            Vec2 min;
            Vec2 max;
        };
        struct
        {
            float fX;
            float fY;
            float fR;
            float fB;
        };
        struct
        {
            float fLeft;
            float fTop;
            float fRight;
            float fBottom;
        };
        float data[4];
    };

public:
    float width() const;
    float height() const;
    void  update();
};
} // namespace Voxol::Math

#endif // VOXOL_MATH_VX_RECT_H