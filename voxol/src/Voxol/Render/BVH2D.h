#ifndef VOXOL_RENDER_BVH2D_H
#define VOXOL_RENDER_BVH2D_H

#include "../Math/VxRect.h"
#include <vector>

namespace Voxol::Render
{

struct BVHNode2D {
    Math::Bounds bounds;
    int32_t      left   = -1;
    int32_t      right  = -1;
    int32_t      start  = 0;
    int32_t      end    = 0;
    bool isLeaf = false;
};

struct BVHItem2D {
    Math::Bounds bounds;
    uint32_t objectId = -1;
};

class BVH2D {
public:
    BVH2D() = default;

    void build(const std::vector<BVHItem2D>& items);
    void query(const Math::Bounds& region, std::vector<int32_t>& outIds) const;
    void queryPoint(const Math::Vec2& point, std::vector<int32_t>& outIds) const;

    inline void markDirty() { m_dirty = true; }
    inline bool isDirty() const { return m_dirty; }

private:
    int buildRecursive(int start, int end, int depth);

    static int longestAxis(const Math::Bounds& b);
    static float centerAlong(const Math::Bounds& b, int axis);

private:
    std::vector<BVHItem2D> m_items;
    std::vector<BVHNode2D> m_nodes;
    bool m_dirty = true;
};

}
#endif