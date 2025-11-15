
#include "BVH2DV1.h"
#include <stack>
#include <algorithm>

namespace Voxol::Scene
{
namespace V1 {

BVH2D::SP BVH2D::make()
{
    auto sp = std::make_shared<BVH2D>();
    return sp;
}

void BVH2D::addItem(const Base::ID::KeyUint64& objectId, const Math::Bounds& bounds)
{
    Item item{};
    item.objectId = objectId;
    item.bounds   = bounds;
    m_items.push_back(item);
    m_objectIdToItem[objectId] = static_cast<uint32_t>(m_items.size() - 1);
}


void BVH2D::build()
{
    m_nodes.clear();
    if (m_items.empty()) return;

    buildRecursive(0, (int)m_items.size());

    rebuildObjectMap();
}


bool BVH2D::updateItemBounds(int itemIndex, const Math::Bounds& newBounds)
{
    if (itemIndex < 0 || itemIndex >= (int)m_items.size()) return false;
    m_items[itemIndex].bounds = newBounds;
    m_items[itemIndex].dirty  = true;
    m_dirty                   = true;
    return true;
}

// -----------------------------
// 更新单个对象的包围盒 (通过 objectId)
// -----------------------------
bool BVH2D::updateItemBoundsByObjectId(const Base::ID::KeyUint64& objectId, const Math::Bounds& newBounds)
{
    auto it = m_objectIdToItem.find(objectId);
    if (it == m_objectIdToItem.end()) return false;
    return updateItemBounds(it->second, newBounds);
}

void BVH2D::updateDirty()
{
    if (!m_dirty) return;

    for (int i = (int)m_nodes.size() - 1; i >= 0; --i)
    {
        Node& node = m_nodes[i];
        if (node.isLeaf())
        {
            const auto& item = m_items[node.itemIndex];
            node.bounds      = item.bounds;
        }
        else
        {
            node.bounds = Math::Bounds::Union(m_nodes[node.left].bounds, m_nodes[node.right].bounds);
        }
    }

    m_dirty = false;
}

void BVH2D::queryPoint(const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const
{
    if (m_nodes.empty()) return;
    queryPointRecursive(0, p, outIds);
}

void BVH2D::queryBounds(const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const
{
    if (m_nodes.empty()) return;
    queryBoundsRecursive(0, b, outIds);
}

void BVH2D::partialRebuild()
{
    const size_t dirtyCount = std::count_if(m_items.begin(), m_items.end(),
                                            [](auto& i) { return i.dirty; });
    float        dirtyRatio = (float)dirtyCount / (float)m_items.size();

    if (dirtyRatio > 0.4f)
    {
        build();
    }
    else
    {
        updateDirty();
    }

    rebuildObjectMap();
}

int BVH2D::buildRecursive(int begin, int end)
{
    Node node{};
    int  nodeIndex = (int)m_nodes.size();
    m_nodes.push_back(node);

    Math::Bounds bounds{};
    //bounds.invalidate();
    bounds.toEmpty();
    for (int i = begin; i < end; ++i)
        bounds.expand(m_items[i].bounds);
    m_nodes[nodeIndex].bounds = bounds;

    int count = end - begin;
    if (count == 1)
    {
        m_nodes[nodeIndex].itemIndex = begin;
        return nodeIndex;
    }

    auto extent = bounds.extent();
    int  axis   = 0;
    if (extent.y > extent.x)
        axis = 1;

    float mid = 0.0f;
    //for (int i = begin; i < end; ++i)
    //    mid += m_items[i].bounds.center()[axis];
    for (int i = begin; i < end; ++i)
    {
        auto&& cv = m_items[i].bounds.center();
        mid += axis > 0 ? cv.y : cv.x;
    }
    mid /= count;

    auto midIter = std::partition(m_items.begin() + begin, m_items.begin() + end,
                                  [axis, mid](const Item& a) {
                                      //return a.bounds.center()[axis] < mid;
                                      auto&& cv = a.bounds.center();
                                      auto v = axis > 0 ? cv.y : cv.x;
                                      return v < mid;
                                  });

    int midIndex = (int)(midIter - m_items.begin());
    if (midIndex == begin || midIndex == end)
        midIndex = begin + (count / 2);

    int left                 = buildRecursive(begin, midIndex);
    int right                = buildRecursive(midIndex, end);
    m_nodes[nodeIndex].left  = left;
    m_nodes[nodeIndex].right = right;
    m_nodes[left].parent     = nodeIndex;
    m_nodes[right].parent    = nodeIndex;

    return nodeIndex;
}

void BVH2D::queryPointRecursive(int nodeIndex, const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const
{
    const Node& node = m_nodes[nodeIndex];
    if (!node.bounds.contains(p))
        return;

    if (node.isLeaf())
    {
        const Item& item = m_items[node.itemIndex];
        if (item.bounds.contains(p))
            outIds.push_back(item.objectId);
    }
    else
    {
        queryPointRecursive(node.left, p, outIds);
        queryPointRecursive(node.right, p, outIds);
    }
}

void BVH2D::queryBoundsRecursive(int nodeIndex, const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const
{
    const Node& node = m_nodes[nodeIndex];
    if (!node.bounds.intersects(b))
        return;

    if (node.isLeaf())
    {
        const Item& item = m_items[node.itemIndex];
        if (item.bounds.intersects(b))
            outIds.push_back(item.objectId);
    }
    else
    {
        queryBoundsRecursive(node.left, b, outIds);
        queryBoundsRecursive(node.right, b, outIds);
    }
}
}
}