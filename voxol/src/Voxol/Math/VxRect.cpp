
#include "VxRect.h"

namespace Voxol::Math
{

float VxRect::width() const
{
    return fRight - fX;
}
float VxRect::height() const
{
    return fBottom - fY;
}
void VxRect::update()
{
}
} // namespace Voxol::Math