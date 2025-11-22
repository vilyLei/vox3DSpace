
#include "BVH2DV2.h"
#include <stack>
#include <algorithm>

namespace Voxol::Scene
{
namespace V2 {

BVH2D_LazyGC::SP BVH2D_LazyGC::make()
{
    return std::make_shared<BVH2D_LazyGC>();
}

// ---------- Add / Build ----------
void BVH2D_LazyGC::addItem(const Base::ID::KeyUint64& objectId, const Math::Bounds& bounds)
{
    LeafTemp lt;
    lt.objectId = objectId;
    lt.bounds   = bounds;
    auto flag   = objectId.isIDValid();
    m_leafTemps.push_back(lt);
}
void BVH2D_LazyGC::build()
{
    m_nodes.clear();
    m_objectToLeaf.clear();
    m_fatBounds.clear();
    m_dirtyLeaves.clear();
    m_deletedCount = 0;
    m_needsCompact = false;

    if (m_leafTemps.empty()) return;

    m_nodes.reserve(std::max<size_t>(4, m_leafTemps.size() * 2));
    // build index array
    std::vector<int> indices((int)m_leafTemps.size());
    for (int i = 0; i < (int)indices.size(); ++i) indices[i] = i;

    int root = buildRecursiveFromLeaves(indices, 0, (int)indices.size(), -1);
    (void)root;

    // init fat bounds from leaves
    for (int i = 0; i < (int)m_nodes.size(); ++i)
    {
        const Node& n = m_nodes[i];

        //printf("v2 build() CCC, n.objectId: %s, n(l=%d,r=%d)\n", n.objectId.idToString().c_str(), n.left, n.right);
        if (isLeaf(n))
        {
            m_fatBounds[n.objectId] = n.bounds.expanded(m_fatPad);
        }
    }

    rebuildObjectMap();
    m_dirty = false;
}


bool BVH2D_LazyGC::updateItemBoundsByObjectId(const Base::ID::KeyUint64& objectId, const Math::Bounds& newBounds)
{
    auto mit = m_objectToLeaf.find(objectId);
    if (mit == m_objectToLeaf.end()) return false;
    int leafIdx = mit->second;
    if (!validNodeIndex(leafIdx)) return false;
    Node& leaf = m_nodes[leafIdx];

    auto fit = m_fatBounds.find(objectId);
    if (fit == m_fatBounds.end())
    {
        m_fatBounds[objectId] = newBounds.expanded(m_fatPad);
        leaf.bounds           = newBounds;
        markAncestorsDirtyUpToRoot(leafIdx);
        m_dirty = true;
        return true;
    }

    Math::Bounds& fb = fit->second;
    // if within fat bound -> update and local refit
    if (newBounds.left() >= fb.left() && newBounds.right() <= fb.right() &&
        newBounds.top() >= fb.top() && newBounds.bottom() <= fb.bottom())
    {
        leaf.bounds = newBounds;
        // immediate local refit (propagate up)
        markAncestorsDirtyUpToRoot(leafIdx);
        m_dirty = true;
        return true;
    }

    // exceeded fat bound: expand fat, mark for partial rebuild
    fb          = newBounds.expanded(m_fatPad);
    leaf.bounds = newBounds;
    m_dirtyLeaves.insert(leafIdx);
    m_dirty = true;
    return true;
}

}
}