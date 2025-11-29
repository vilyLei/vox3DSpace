
#include "PositionDistributor.h"

namespace Voxol::Scene::Layout
{
namespace Honeycomb
{
Math::Vec2 hexToWorld(Hex h, float hexRadius)
{
    float size = hexRadius; // distance center->vertex
    float x    = size * std::sqrt(3.f) * (h.q + h.r * 0.5f);
    float y    = size * 1.5f * h.r;
    return {x, y};
}
Math::Vec2 hexToWorld2(int q, int r, float hexR)
{
    float x = hexR * std::sqrt(3.0f) * (q + r * 0.5f);
    float y = hexR * 1.5f * r;
    return {x, y};
}

std::vector<Math::Vec2> generate(
    Math::Vec2 center,
    size_t     count,
    float      hexRadius
)
{
    std::vector<Math::Vec2> pts;
    pts.reserve(count);

    if (count == 0) return pts;

    // 1. Push center first
    pts.push_back(center);
    if (count == 1) return pts;

    size_t total  = 1;
    int    radius = 1;

    // 2. Expand ring by ring until enough points
    while (total < count)
    {
        // starting hex: (q = 0, r = -radius)
        Hex h = {0, -radius};

        // Move to first ring direction start
        h.q += dq[4] * radius;
        h.r += dr[4] * radius;

        // 6 sides
        for (int dir = 0; dir < 6; ++dir)
        {
            for (int step = 0; step < radius; ++step)
            {
                if (total >= count) break;

                // world position
                Math::Vec2 p = hexToWorld(h, hexRadius);
                pts.push_back({center.x + p.x, center.y + p.y});
                total++;

                // move one step along direction
                h.q += dq[dir];
                h.r += dr[dir];
            }
            if (total >= count) break;
        }

        radius++;
    }

    return pts;
}
std::vector<Math::Vec2> generateHexGridT(const Math::Vec2& center, int rings, float hexR)
{
    std::vector<Math::Vec2> ls;
    ls.reserve(1 + 3 * rings * (rings + 1));
    for (int q = -rings; q <= rings; ++q)
    {
        for (int r = -rings; r <= rings; ++r)
        {
            int s = -q - r;
            if (std::abs(q) + std::abs(r) + std::abs(s) <= rings * 2)
            {
                auto v = hexToWorld2(q, r, hexR);
                ls.push_back(center + v);
            }
        }
    }

    return ls;
}
} // namespace Honeycomb

std::vector<Math::Vec2> PositionDistribution::hexagonalGrid(int count, const Math::Vec2& center, int rings, float hexRadius)
{
    return Honeycomb::generateHexGridT(center, rings, hexRadius);
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