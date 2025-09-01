
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
            float fRX;
            float fBY;
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
    static VxRect makeWH(float pw, float ph);
    static VxRect makeXYWH(float px, float py, float pw, float ph);
    static VxRect makeLTRB(float pl, float pt, float pr, float pb);

    void setXY(float px, float py);
    void setWH(float pw, float ph);
    void setXYWH(float px, float py, float pw, float ph);
    void setLTRB(float pl, float pt, float pr, float pb);

    
    void setSize(float pw, float ph);
    bool intersects(const VxRect& other) const;
    void outset(float dx, float dy);
    bool contains(float px, float py) const;

    float x() const;
    float y() const;
    float left() const;
    float top() const;
    float right() const;
    float bottom() const;

    float width() const;
    float height() const;
    void  update();
    bool isEmpty() const;
};
} // namespace Voxol::Math

#endif // VOXOL_MATH_VX_RECT_H