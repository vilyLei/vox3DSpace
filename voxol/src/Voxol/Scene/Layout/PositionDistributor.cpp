
#include "PositionDistributor.h"

namespace Voxol::Scene::Layout
{
namespace Distribution
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
    float      hexRadius)
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
std::vector<Math::Vec2> generateHexGrid(const Math::Vec2& center, int rings, float hexR)
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
Math::Vec2 diamond(int i, int n, float R)
{
    float t     = float(i) / float(n); // 0~1
    float angle = t * 4.0f * MATH_PI;  // four segments

    float s = std::fmod(angle, MATH_2PI) / MATH_2PI;

    // four directions£ºright, top, left, down
    int sector = int(angle / MATH_2PI) % 4;

    switch (sector)
    {
        case 0: return {R, R * (1 - 2 * s)};
        case 1: return {R * (1 - 2 * s), R};
        case 2: return {-R, R * (2 * s - 1)};
        case 3: return {R * (2 * s - 1), -R};
    }
    return {};
}
std::vector<Math::Vec2> kochSnowflake(int iteration, float R)
{
    std::vector<Math::Vec2> pts{
        {0, -R},
        {R * 0.866f, 0.5f * R},
        {-R * 0.866f, 0.5f * R}};

    for (int it = 0; it < iteration; ++it)
    {
        std::vector<Math::Vec2> next;
        for (int i = 0; i < pts.size(); ++i)
        {
            Math::Vec2 a = pts[i];
            Math::Vec2 b = pts[(i + 1) % pts.size()];

            Math::Vec2 ab = (b - a) / 3.0f;

            Math::Vec2 p1 = a + ab;
            Math::Vec2 p2 = a + ab * 2.0f;

            float      angle = MATH_PI / 3.0f;
            Math::Vec2 peak  = {
                p1.x + ab.x * cos(angle) - ab.y * sin(angle),
                p1.y + ab.x * sin(angle) + ab.y * cos(angle)};

            next.push_back(a);
            next.push_back(p1);
            next.push_back(peak);
            next.push_back(p2);
        }
        pts = std::move(next);
    }
    return pts;
}

Math::Vec2 rhombusGrid(int i, int n, const Math::Vec2& spacing)
{
    int side = std::ceil(std::sqrt(n));
    int x    = i % side;
    int y    = i / side;

    return {
        (x - y) * spacing.x,
        (x + y) * spacing.y * 0.5f};
}
Math::Vec2 rose(int i, int n, float R, float amplitude)
{
    float t = float(i) / n;
    float a = t * 2 * MATH_PI;

    float r = R * std::cos(amplitude * a);
    return {r * cos(a), r * sin(a)};
}
} // namespace Distribution

namespace Superformula
{

inline float superformulaRadius(float theta,
                                float m,
                                float a,
                                float b,
                                float n1,
                                float n2,
                                float n3)
{
    float t1 = std::cos(m * theta / 4.0f) / a;
    float t2 = std::sin(m * theta / 4.0f) / b;

    t1 = std::pow(std::abs(t1), n2);
    t2 = std::pow(std::abs(t2), n3);

    float r = std::pow(t1 + t2, -1.0f / n1);
    return r;
}
Math::Vec2 superformulaPoint(int i, int count, float m, float a, float b, float n1, float n2, float n3, float scale)
{
    float t     = float(i) / float(count);
    float theta = t * 2.0f * MATH_PI;

    float r = superformulaRadius(theta, m, a, b, n1, n2, n3);

    return {
        scale * r * std::cos(theta),
        scale * r * std::sin(theta)};
}
std::vector<Math::Vec2> generateSuperformula(
    int   count,
    const FormulaParam& param,
    float scale)
{
    std::vector<Math::Vec2> pts;
    pts.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        pts.push_back(
            superformulaPoint(i, count, param.m, param.a, param.b, param.n1, param.n2, param.n3, scale));
    }
    return pts;
}

}
std::vector<Math::Vec2> PositionDistribution::hexagonalGrid(int count, const Math::Vec2& center, int rings, float hexRadius)
{
    return Distribution::generateHexGrid(center, rings, hexRadius);
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

std::vector<Math::Vec2> PositionDistribution::spiral(std::vector<float>& angles, int count, const Math::Vec2& center, float startRadius, float radiusStep, float startRad, float radStep)
{
    std::vector<Math::Vec2> positions;
    positions.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        float radius = startRadius + radiusStep * i;
        float angle  = startRad + radStep * i;
        angles.push_back(angle);
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