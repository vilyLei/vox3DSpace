#ifndef VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H
#define VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H

#include "IPositionDistribution.h"

namespace Voxol::Scene::Layout
{
namespace HexLayout
{

// axial six directions
static constexpr int dq[6] = {+1, +1, 0, -1, -1, 0};
static constexpr int dr[6] = {0, -1, -1, 0, +1, +1};

struct Hex
{
    int q, r;
};

inline Hex add(Hex a, Hex b) { return {a.q + b.q, a.r + b.r}; }

// axial -> world (pointy-top hexes)
Math::Vec2 hexToWorld(Hex h, float hexRadius);
Math::Vec2 hexToWorld2(int q, int r, float hexR);

/**
 * center   : world center point
 * count    : number of points to generate
 * hexRadius: distance between cell centers (recommend point spacing)
 */
std::vector<Math::Vec2> generate(
    Math::Vec2 center,
    size_t     count,
    float      hexRadius = 20.0f // spacing
);
inline std::vector<Math::Vec2> generateHexGridT(const Math::Vec2& center, int rings, float hexR);

} // namespace HexLayout
class PositionDistribution
{
public:
    static std::vector<Math::Vec2> hexagonalGrid(int count, const Math::Vec2& center, int rings, float hexRadius);
    static std::vector<Math::Vec2> circle(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0);
    static std::vector<Math::Vec2> grid(int count, const Math::Vec2& start, const Math::Vec2& spacing = {10, 10}, int columns = 4, const Math::Vec2& itemSize = {}, const Math::Vec2& staggered = {});
    static std::vector<Math::Vec2> spiral(std::vector<float>& angles, int count, const Math::Vec2& center, float startRadius = 10, float radiusStep = 5, float radStep = 0.5f);
    static std::vector<Math::Vec2> arc(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0, float arcRadian = 3.14159f);
};
} // namespace Voxol::Scene::Layout
#endif