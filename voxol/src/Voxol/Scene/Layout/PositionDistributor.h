#ifndef VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H
#define VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H

#include "IPositionDistribution.h"

namespace Voxol::Scene::Layout
{
class PositionDistribution
{
public:
    static std::vector<Math::Vec2> hexagonalGrid(int count, const Math::Vec2& start, float hexSize, int columns);
    static std::vector<Math::Vec2> circle(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0);
    static std::vector<Math::Vec2> grid(int count, const Math::Vec2& start, const Math::Vec2& spacing = {10, 10}, int columns = 4, const Math::Vec2& itemSize = {}, const Math::Vec2& staggered = {});
    static std::vector<Math::Vec2> spiral(int count, const Math::Vec2& center, float startRadius = 10, float radiusStep = 5);
    static std::vector<Math::Vec2> arc(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0, float arcRadian = 3.14159f);
};
} // namespace Voxol::Scene::Layout
#endif