
#ifndef VOXOL_MATH_VX_RECT_H
#define VOXOL_MATH_VX_RECT_H

#include "Vec2.h"
#include "Mat33.h"
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
        float data[4]{};
    };

public:
    constexpr VxRect() = default;
    constexpr VxRect(float x, float y, float right, float bottom) :
        fX(x), fY(y), fRX(right), fBY{bottom} {}

public:
    static VxRect makeWH(float pw, float ph);
    static VxRect makeXYWH(float px, float py, float pw, float ph);
    static VxRect makeLTRB(float pl, float pt, float pr, float pb);

    void setXY(float px, float py);
    void setWH(float pw, float ph);
    void setXYWH(float px, float py, float pw, float ph);
    void setLTRB(float pl, float pt, float pr, float pb);

    void mat33MapTo(const Mat33& mat3, VxRect& dst);
    void addXY(float px, float py);


    void setSize(float pw, float ph);
    bool intersects(const VxRect& other) const;
    void outset(float dx, float dy);
    bool contains(float px, float py) const;
    bool contains(const Vec2& pv) const;

    float x() const;
    float y() const;
    float left() const;
    float top() const;
    float right() const;
    float bottom() const;

    float width() const;
    float height() const;
    void  toEmpty(float px = 0, float py = 0);
    bool  isEmpty() const;

    inline float centerX() const { return (fX + fRX) * 0.5f; }
    inline float centerY() const { return (fY + fBY) * 0.5f; }
    inline Vec2  center() const { return (min + max) * 0.5f; }
    inline Vec2  extent() const { return max - min; }

    inline void expand(const VxRect& other)
    {
        min.x = std::min(min.x, other.min.x);
        min.y = std::min(min.y, other.min.y);
        max.x = std::max(max.x, other.max.x);
        max.y = std::max(max.y, other.max.y);
    }

    int   longestAxis() const;
    float centerAlong(int axis) const;
    void  print() const;

    static VxRect Union(const VxRect& a, const VxRect& b);
};
using Bounds = VxRect;

} // namespace Voxol::Math

#endif // VOXOL_MATH_VX_RECT_H