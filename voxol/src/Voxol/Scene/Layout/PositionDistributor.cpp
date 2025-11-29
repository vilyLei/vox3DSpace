
#include "PositionDistributor.h"

namespace Voxol::Scene::Layout
{

std::vector<Math::Vec2> PositionDistribution::circle(int count, const Math::Vec2& center, float radius, float startRadian)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float angle = startRadian + MATH_2PI * float(i) / count;
        positions.emplace_back(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle));
    }
    return positions;
}

std::vector<Math::Vec2> PositionDistribution::grid(int count, const Math::Vec2& start, const Math::Vec2& spacing, int columns, const Math::Vec2& itemSize)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    auto dv = itemSize + spacing;
    for (int i = 0; i < count; ++i)
    {
        int col = i % columns;
        int row = i / columns;
        positions.emplace_back(
            start.x + col * dv.x,
            start.y + row * dv.y);
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

std::vector<Math::Vec2> PositionDistribution::arc(int count, const Math::Vec2& center, float radius, float startRadian, float arcRadian)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float angle = startRadian + arcRadian * float(i) / std::max(1, count - 1);
        positions.emplace_back(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle));
    }
    return positions;
}
}