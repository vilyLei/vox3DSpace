#include "BVH2D.h"
#include <stack>
#include <algorithm>

namespace Voxol::Scene
{
    
int BVH2D::longestAxis(const Math::Bounds& b)
{
    auto ext = b.extent();
    return (ext.x > ext.y) ? 0 : 1;
}

float BVH2D::centerAlong(const Math::Bounds& b, int axis)
{
    auto c = b.center();
    return (axis == 0) ? c.x : c.y;
}

void BVH2D::build(const std::vector<BVHItem2D>& items)
{
    if (items.empty())
        return;

    m_items = items;
    m_nodes.clear();
    m_nodes.reserve(items.size() * 2);

    buildRecursive(0, (int)items.size(), 0);
    m_dirty = false;
}

int BVH2D::buildRecursive(int start, int end, int depth)
{
    auto bounds = m_items[start].bounds;
    for (int i = start + 1; i < end; i++)
        bounds.expand(m_items[i].bounds);

    int nodeIdx = (int)m_nodes.size();
    m_nodes.emplace_back();
    BVHNode2D& node = m_nodes.back();
    node.bounds = bounds;

    int count = end - start;
    if (count <= 4) {
        node.start = start;
        node.end = end;
        node.isLeaf = true;
        return nodeIdx;
    }

    int axis = longestAxis(bounds);
    int mid = (start + end) / 2;

    std::nth_element(
        m_items.begin() + start,
        m_items.begin() + mid,
        m_items.begin() + end,
        [axis](const BVHItem2D& a, const BVHItem2D& b) {
            return centerAlong(a.bounds, axis) < centerAlong(b.bounds, axis);
        });

    node.left = buildRecursive(start, mid, depth + 1);
    node.right = buildRecursive(mid, end, depth + 1);
    return nodeIdx;
}

void BVH2D::query(const Math::Bounds& region, std::vector<int32_t>& outIds) const
{
    if (m_nodes.empty())
        return;

    std::stack<int32_t> stack;
    stack.push(0);

    while (!stack.empty()) {
        int32_t idx = stack.top();
        stack.pop();

        const BVHNode2D& node = m_nodes[idx];
        if (!region.intersects(node.bounds))
            continue;

        if (node.isLeaf) {
            for (int i = node.start; i < node.end; i++) {
                if (region.intersects(m_items[i].bounds))
                    outIds.push_back(m_items[i].objectId);
            }
        } else {
            if (node.left >= 0) stack.push(node.left);
            if (node.right >= 0) stack.push(node.right);
        }
    }
}

void BVH2D::queryPoint(const Math::Vec2& point, std::vector<int32_t>& outIds) const
{
    if (m_nodes.empty())
        return;

    std::stack<int> stack;
    stack.push(0);

    while (!stack.empty()) {
        int idx = stack.top();
        stack.pop();

        const BVHNode2D& node = m_nodes[idx];
        if (!node.bounds.contains(point))
            continue;

        if (node.isLeaf) {
            for (int i = node.start; i < node.end; i++) {
                if (m_items[i].bounds.contains(point))
                    outIds.push_back(m_items[i].objectId);
            }
        } else {
            if (node.left >= 0) stack.push(node.left);
            if (node.right >= 0) stack.push(node.right);
        }
    }
}
}