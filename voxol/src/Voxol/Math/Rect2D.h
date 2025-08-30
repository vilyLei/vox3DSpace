
#ifndef VOXOL_MATH_RECT2D_H
#define VOXOL_MATH_RECT2D_H

#include <cmath>

namespace Voxol::Math
{

struct Rect2D
{
public:
    union
    {
        union
        {
            union
            {
                struct
                {
                    float fX;
                    float fY;
                };
                struct
                {
                    float fLeft;
                    float fTop;
                };
            };
            float fRight;
            float fBottom;
        };
        float data[4];
    };

public:
    float width() const;
    float height() const;
    void update();
};
} // namespace Voxol::Math

#endif // VOXOLMATH_RECT2D_H