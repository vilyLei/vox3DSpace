#ifndef VOXOL_INTERSECTION_SYSTEM_H
#define VOXOL_INTERSECTION_SYSTEM_H

#include "Component.h"
#include "ComponentStorage.h"
#include <cmath>

class IntersectionSystem {
public:
    static bool rect_circle_intersect(const Rect& r, const Circle& c);
    // 检测所有 Rect + Circle 的组合
    static int count_intersections(
        const ComponentStorage<Rect>& rects,
        const ComponentStorage<Circle>& circles
    );
};

#endif  // VOXOL_INTERSECTION_SYSTEM_H