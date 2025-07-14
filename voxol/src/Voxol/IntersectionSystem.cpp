#include "IntersectionSystem.h"

bool IntersectionSystem::rect_circle_intersect(const Rect &r, const Circle &c) {
  float left = r.x;
  float right = r.x + r.width;
  float top = r.y;
  float bottom = r.y + r.height;

  float closestX = std::fmax(left, std::fmin(c.cx, right));
  float closestY = std::fmax(top, std::fmin(c.cy, bottom));

  float dx = c.cx - closestX;
  float dy = c.cy - closestY;
  return (dx * dx + dy * dy) <= (c.radius * c.radius);
}

// 检测所有 Rect + Circle 的组合
int IntersectionSystem::count_intersections(
    const ComponentStorage<Rect> &rects,
    const ComponentStorage<Circle> &circles) {
  int count = 0;
  for (const auto &[re, r] : rects.all()) {
    for (const auto &[ce, c] : circles.all()) {
      if (rect_circle_intersect(r, c))
        count++;
    }
  }
  return count;
}