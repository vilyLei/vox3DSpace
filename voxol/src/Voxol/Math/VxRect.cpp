
#include "VxRect.h"
#include <cstdio>
#include <limits>

namespace Voxol::Math
{

VxRect VxRect::makeWH(float pw, float ph)
{
    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;
    return VxRect(0, 0, pw, ph);
};
VxRect VxRect::makeXYWH(float px, float py, float pw, float ph)
{
    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;
    return {px, py, px + pw, py + ph};
}
VxRect VxRect::makeLTRB(float pl, float pt, float pr, float pb)
{
    auto pw = pr - pl;
    auto ph = pb - pt;

    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;

    return VxRect{pl, pt, pl + pw, pt + ph};
}

void VxRect::moveTo(float px, float py)
{
    //setXYWH(px, py, width(), height());
    fRX += px - fX;
    fBY += py - fY;
    fX = px;
    fY = py;
}
void VxRect::offset(float dx, float dy)
{
    //setXYWH(px, py, width(), height());
    fRX += dx;
    fBY += dy;
    fX += dx;
    fY += dy;
}

void VxRect::setWH(float pw, float ph)
{

    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;

    fRight  = fX + pw;
    fBottom = fY + ph;
}

void VxRect::setXYWH(float px, float py, float pw, float ph)
{

    fX = px;
    fY = py;

    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;

    fRight  = fX + pw;
    fBottom = fY + ph;
}

void VxRect::setLTRB(float pl, float pt, float pr, float pb)
{
    fX = pl;
    fY = pt;

    auto pw = pr - pl;
    auto ph = pb - pt;

    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;

    fRight  = fX + pw;
    fBottom = fY + ph;
}


void VxRect::mat33MapTo(const Mat33& mat3, VxRect& dst) const
{
    auto pv = mat3.mapXY(fX, fY);
    dst.toEmpty(pv);
    pv = mat3.mapXY(fRX, fY);
    dst.addXY(pv);
    pv = mat3.mapXY(fRX, fBY);
    dst.addXY(pv);
    pv = mat3.mapXY(fX, fBY);
    dst.addXY(pv);
}
void VxRect::addXY(float px, float py)
{
    if (fLeft > px) fLeft = px;
    if (fRight < px) fRight = px;
    if (fTop > py) fTop = py;
    if (fBottom < py) fBottom = py;
}
void VxRect::addXY(const Vec2& p)
{
    if (fLeft > p.x) fLeft = p.x;
    if (fRight < p.x) fRight = p.x;
    if (fTop > p.y) fTop = p.y;
    if (fBottom < p.y) fBottom = p.y;
}

Vec2 VxRect::size() const
{
    return {fRight - fLeft, fBottom - fTop};
}
void VxRect::size(const Vec2& vsize)
{
    size(vsize.x, vsize.y);
}
void VxRect::size(float pw, float ph)
{
    pw = pw > 0 ? pw : 0;
    ph = ph > 0 ? ph : 0;

    fRight  = fX + pw;
    fBottom = fY + ph;
}
bool VxRect::intersects(const VxRect& other) const
{
    if (other.fX > fRight || other.fRight < fLeft)
        return false;
    if (other.fY > fBottom || other.fBottom < fY)
        return false;
    return true;
}
void VxRect::outset(float dx, float dy)
{
    fX -= dx;
    fY -= dy;
    fRight += dx;
    fBottom += dy;
}

void VxRect::floatToRound()
{
    fX = std::roundf(fX);
    fY = std::roundf(fY);
    fRight = std::roundf(fRight);
    fBottom = std::roundf(fBottom);
}

bool VxRect::contains(float px, float py) const
{
    if (px < fLeft || px > fRight)
        return false;
    if (py < fTop || py > fBottom)
        return false;
    return true;
}
bool VxRect::contains(const Vec2& pv) const
{
    return contains(pv.x, pv.y);
}

float VxRect::x() const { return fX; }
float VxRect::y() const { return fY; }
float VxRect::left() const { return fLeft; }
float VxRect::top() const { return fTop; }
float VxRect::right() const { return fRight; }
float VxRect::bottom() const { return fBottom; }


float VxRect::width() const
{
    return fRight - fLeft;
}
float VxRect::height() const
{
    return fBottom - fTop;
}

void VxRect::toEmpty(const Vec2& p) {

    fX = fRX = p.x;
    fY = fBY = p.y;
}
void VxRect::toEmpty()
{
    toLimit();
}

bool VxRect::isEmpty() const
{
    return width() <= 0 || height() <= 0;
}

void VxRect::toLimit()
{
    fX = fY = std::numeric_limits<float>::max();
    fRX = fBY = -fX;
}

int VxRect::longestAxis() const
{
    auto&& ext = extent();
    return (ext.x > ext.y) ? 0 : 1; /// 0 = x, 1 = y
}
float VxRect::centerAlong(int axis) const
{
    auto&& c = center();
    return (axis == 0) ? c.x : c.y;
}

void VxRect::print() const
{
    printf("VxRect(x=%f,y=%f,w=%f,h=%f)\n", fX, fY, width(), height());
}
VxRect VxRect::Union(const VxRect& a, const VxRect& b)
{
    VxRect c = a;
    c.expand(b);
    return c;
}
} // namespace Voxol::Math