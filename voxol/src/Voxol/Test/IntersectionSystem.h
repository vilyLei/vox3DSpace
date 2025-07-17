#ifndef VOXOL_INTERSECTION_SYSTEM_H
#define VOXOL_INTERSECTION_SYSTEM_H

#include "TestComponent.h"
#include "TestComponentStorage.h"
#include <cmath>
namespace Voxol::Test
{
class IntersectionSystem
{
public:
    static bool rect_circle_intersect(const Rect& r, const Circle& c);
    // 检测所有 Rect + Circle 的组合
    static int count_intersections(
        const TestComponentStorage<Rect>&   rects,
        const TestComponentStorage<Circle>& circles);
};
} // namespace Voxol::Test
#endif // VOXOL_INTERSECTION_SYSTEM_H