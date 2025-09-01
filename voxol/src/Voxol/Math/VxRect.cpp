
#include "VxRect.h"

namespace Voxol::Math
{


void VxRect::setXY(float px, float py)
{

    fX = px;
    fY = py;
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

void VxRect::setSize(float pw, float ph)
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

bool VxRect::contains(float px, float py) const
{
    if (px < fLeft || px > fRight)
        return false;
    if (py < fTop || py > fBottom)
        return false;
    return true;
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
void VxRect::update()
{
}

bool VxRect::isEmpty() const
{
    return width() <= 0 || height() <= 0;
}
} // namespace Voxol::Math