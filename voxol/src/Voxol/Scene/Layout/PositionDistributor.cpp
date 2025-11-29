
#include "PositionDistributor.h"

namespace Voxol::Scene::Layout
{

std::vector<Math::Vec2> PositionDistribution::hexagonalGrid(int count, const Math::Vec2& start, float hexSize, int columns)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    float hexWidth  = hexSize * 2.0f;
    float hexHeight = hexSize * std::sqrt(3.0f);

    for (int i = 0; i < count; ++i)
    {
        int col = i % columns;
        int row = i / columns;

        float xOffset = (row % 2 == 1) ? hexWidth * 0.5f : 0.0f;
        //xOffset       = 0;
        positions.emplace_back(
            start.x + col * hexWidth * 0.75f + xOffset,
            start.y + row * hexHeight * 0.5f);
    }
    return positions;
}
std::vector<Math::Vec2> PositionDistribution::circle(std::vector<float>& angles, int count, const Math::Vec2& center, float radius, float startRadian)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float angle = startRadian + MATH_2PI * float(i) / count;
        angles.push_back(angle);
        positions.emplace_back(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle));
    }
    return positions;
}

std::vector<Math::Vec2> PositionDistribution::grid(int count, const Math::Vec2& start, const Math::Vec2& spacing, int columns, const Math::Vec2& itemSize, const Math::Vec2& staggered)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    auto dv = itemSize + spacing;
    for (int i = 0; i < count; ++i)
    {
        int col = i % columns;
        int row = i / columns;

        float xOffset = ((row % 2 == 1) ? dv.x : 0) * staggered.x;
        float yOffset = ((col % 2 == 1) ? dv.y : 0) * staggered.y;

        positions.emplace_back(
            start.x + xOffset + col * dv.x,
            start.y + yOffset + row * dv.y);
    }
    return positions;
}

std::vector<Math::Vec2> PositionDistribution::spiral(int count, const Math::Vec2& center, float startRadius, float radiusStep)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float radius = startRadius + radiusStep * i;
        float angle  = (MATH_2PI / 6.0f) * i;
        positions.emplace_back(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle));
    }
    return positions;
}

std::vector<Math::Vec2> PositionDistribution::arc(std::vector<float>& angles, int count, const Math::Vec2& center, float radius, float startRadian, float arcRadian)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float angle = startRadian + arcRadian * float(i) / std::max(1, count - 1);
        angles.push_back(angle);
        positions.emplace_back(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle));
    }
    return positions;
}
} // namespace Voxol::Scene::Layout