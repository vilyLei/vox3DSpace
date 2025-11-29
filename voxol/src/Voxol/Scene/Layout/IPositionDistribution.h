#ifndef VOXOL_SCENE_LAYOUT_I_POSITION_DISTRIBUTION_H
#define VOXOL_SCENE_LAYOUT_I_POSITION_DISTRIBUTION_H

#include "../Math/VxRect.h"
#include "../Math/MathDef.h"
#include <vector>

namespace Voxol::Scene::Layout
{
class IPositionDistribution
{
public:
    virtual ~IPositionDistribution()                     = default;
    virtual std::vector<Math::Vec2> calculate(int count) = 0;
    virtual const char*             getName() const      = 0;
};
} // namespace Voxol::Scene::Layout
#endif