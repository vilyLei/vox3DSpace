
#include "Rect2D.h"

namespace Voxol::Math
{

float Rect2D::width() const
{
    return fRight - fX;
}
float Rect2D::height() const
{
    return fBottom - fY;
}
void Rect2D::update()
{
}
} // namespace Voxol::Math