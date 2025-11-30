#ifndef VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H
#define VOXOL_SCENE_LAYOUT_POSITION_DISTRIBUTION_H

#include "IPositionDistribution.h"

namespace Voxol::Scene::Layout
{
namespace Distribution
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
inline std::vector<Math::Vec2> generateHexGrid(const Math::Vec2& center, int rings, float hexR);
Math::Vec2                     diamond(int i, int n, float R);
std::vector<Math::Vec2>        kochSnowflake(int iteration, float R);
Math::Vec2                     rhombusGrid(int i, int n, const Math::Vec2& spacing);
Math::Vec2                     rose(int i, int n, float R, float amplitude);

} // namespace Distribution
namespace Superformula
{
struct FormulaParam
{
    float m  = 6;
    float a = 1;
    float b = 1;
    float n1 = 0.3;
    float n2 = 0.3;
    float n3 = 0.3;
};

std::vector<Math::Vec2> generateSuperformula(
    int                 count,
    const FormulaParam& param,
    float               scale = 1);
}
class PositionDistribution
{
public:
    static std::vector<Math::Vec2> hexagonalGrid(int count, const Math::Vec2& center, int rings, float hexRadius);
    static std::vector<Math::Vec2> circle(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0);
    static std::vector<Math::Vec2> grid(int count, const Math::Vec2& start, const Math::Vec2& spacing = {10, 10}, int columns = 4, const Math::Vec2& itemSize = {}, const Math::Vec2& staggered = {});
    static std::vector<Math::Vec2> spiral(std::vector<float>& angles, int count, const Math::Vec2& center, float startRadius = 10, float radiusStep = 5, float startRad = 0, float radStep = 0.5f);
    static std::vector<Math::Vec2> arc(std::vector<float>& angles, int count, const Math::Vec2& center, float radius = 50, float startRadian = 0, float arcRadian = 3.14159f);

};
} // namespace Voxol::Scene::Layout
#endif