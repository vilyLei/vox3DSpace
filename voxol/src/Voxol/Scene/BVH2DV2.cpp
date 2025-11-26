
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

bool BVH2D_LazyGC::removeItemByObjectId(const Base::ID::KeyUint64& objectId)
{
    auto it = m_objectToLeaf.find(objectId);
    if (it == m_objectToLeaf.end()) return false;
    int leafIdx = it->second;
    if (!validNodeIndex(leafIdx))
    {
        m_objectToLeaf.erase(it);
        return false;
    }

    Node& leaf = m_nodes[leafIdx];
    // mark as deleted (logical delete)
    leaf.objectId = Base::ID::INVALID_KEY;

    // remove object mapping and fat bound entry if exist
    m_objectToLeaf.erase(it);
    m_fatBounds.erase(objectId);

    // add this slot to free list for future reuse
    m_freeList.push_back(leafIdx);
    ++m_deletedCount;
    m_needsCompact = true;

    // propagate up: 如果父节点变成两个无效子，则将父也转换为无效叶槽（objectId=-1, left=right=-1）
    int cur = leaf.parent;
    while (cur >= 0)
    {
        Node& p = m_nodes[cur];

        bool leftAlive  = (p.left >= 0) && ((isLeafNodeSlot(m_nodes[p.left]) && m_nodes[p.left].objectId.isIDValid()) || !isLeafNodeSlot(m_nodes[p.left]));
        bool rightAlive = (p.right >= 0) && ((isLeafNodeSlot(m_nodes[p.right]) && m_nodes[p.right].objectId.isIDValid()) || !isLeafNodeSlot(m_nodes[p.right]));

        if (!leftAlive && !rightAlive)
        {
            // collapse parent to an empty leaf slot
            p.left = p.right = -1;
            p.objectId       = Base::ID::INVALID_KEY;
            cur              = p.parent;
        }
        else
        {
            break;
        }
    }

    // 标记树需要 refit（但 refit 会把已删除叶当空处理，不会出错）
    m_dirty = true;
    return true;
}


// 每帧在 endFrameCompact 调用：处理至多 maxBlocksToProcess 个 block
void BVH2D_LazyGC::endFrameCompact(size_t maxBlocksToProcess)
{
    if (!m_needsCompact && m_deletedCount == 0) return;

    size_t live      = m_objectToLeaf.size();
    size_t deleted   = m_deletedCount;
    float  holeRatio = (live + deleted) ? (float)deleted / float(live + deleted) : 0.0f;

    // 优先处理一些 dirty blocks（逐块压缩）
    size_t processed = 0;
    auto   it        = m_dirtyBlocks.begin();
    while (it != m_dirtyBlocks.end() && processed < maxBlocksToProcess)
    {
        int blockIndex = *it;
        compactBlock(blockIndex);
        it = m_dirtyBlocks.erase(it);
        ++processed;
    }

    // 若空洞比例依然很高或者 node 数量 >> ideal * factor，则做全量 compact
    size_t idealNodes = std::max<size_t>(4, live * 2);
    if (holeRatio > m_deleteRebuildRatio || m_nodes.size() > idealNodes * m_compactFactor)
    {
        // 全量重建(安全)
        compactIfNeededFull();
    }
}

void BVH2D_LazyGC::compactBlock(int blockIndex)
{
    int start = blockIndex * GC_BLOCK_SIZE;
    int end   = std::min((int)m_nodes.size(), start + GC_BLOCK_SIZE);

    // collect live leaves within this block
    std::vector<std::pair<int, int>> livePairs; // pair(oldLeafIdx, newLeafSlotCandidate)
    livePairs.reserve(end - start);
    for (int i = start; i < end; ++i)
    {
        if (!validNodeIndex(i)) continue;
        Node& n = m_nodes[i];
        if (isValidLeaf(n))
        {
            livePairs.emplace_back(i, -1);
        }
        else if (isLeafNodeSlot(n) && n.objectId.isIDInvalid())
        {
            // deleted slot — candidate for reuse
            m_freeList.push_back(i);
        }
    }
    // If block has no live leaves, return (we already pushed deleted slots into freeList)
    if (livePairs.empty()) return;

    // Heuristic simple approach:
    // We'll not physically move nodes in m_nodes (避免内部指针复杂度)；
    // 而是 mark them as "live" and allow future partial rebuilds to append new nodes,
    // 或者在全量 compact 时再做彻底重排。
    //
    // 这里我们只是 attempt to reclaim obviously-deleted slots in the block by filling freeList,
    // real relocation is expensive and left to compactIfNeededFull().
    //
    // So this compactBlock is intentionally lightweight to avoid big work per frame.
    //
    return;
}

void BVH2D_LazyGC::compactIfNeededFull()
{
    std::vector<LeafTemp> saved;
    saved.reserve(m_objectToLeaf.size());
    for (const auto& kv : m_objectToLeaf)
    {
        int leafIdx = kv.second;
        if (!validNodeIndex(leafIdx)) continue;
        const Node& n = m_nodes[leafIdx];
        if (n.objectId.isIDValid()) saved.push_back(LeafTemp{n.objectId, n.bounds});
    }
    // replace leafTemps and rebuild
    m_leafTemps.swap(saved);
    // reset
    m_nodes.clear();
    m_objectToLeaf.clear();
    m_fatBounds.clear();
    m_dirtyLeaves.clear();
    m_freeList.clear();
    m_deletedCount = 0;
    m_needsCompact = false;

    // call build which will repopulate nodes & mapping
    build();
}

void BVH2D_LazyGC::updateDirty()
{
    if (!m_dirty && m_dirtyLeaves.empty()) return;
    if (m_nodes.empty())
    {
        m_dirty = false;
        m_dirtyLeaves.clear();
        return;
    }

    size_t leafCount  = m_objectToLeaf.size();
    size_t dirtyCount = m_dirtyLeaves.size();

    // if many dirty -> full rebuild
    float ratio = leafCount ? (float)dirtyCount / float(leafCount) : 0.0f;
    if (ratio > m_rebuildRatio)
    {
        // rebuild full from current live leaves
        collectLeavesToTempsAndRebuild();
        return;
    }

    // else handle partial rebuilds
    std::vector<int> dirtyList;
    dirtyList.reserve(m_dirtyLeaves.size());
    for (int li : m_dirtyLeaves) dirtyList.push_back(li);

    for (int leafIdx : dirtyList)
    {
        if (!validNodeIndex(leafIdx)) continue;
        const Node& maybeLeaf = m_nodes[leafIdx];
        if (!isLeaf(maybeLeaf) || maybeLeaf.objectId.isIDInvalid()) continue;
        int subtreeRoot = chooseSubtreeRootForLeaf(leafIdx);
        rebuildSubtreeAtNode(subtreeRoot);
    }

    m_dirtyLeaves.clear();

    // recompute bounds bottom-up
    refitAllNodes();

    // rebuild mapping (re-maps appended nodes)
    rebuildObjectMap();

    m_dirty = false;
}

void BVH2D_LazyGC::compactIfNeeded()
{
    // collect current live leaves
    std::vector<LeafTemp> saved;
    saved.reserve(m_objectToLeaf.size());
    for (const auto& kv : m_objectToLeaf)
    {
        int leafIdx = kv.second;
        if (!validNodeIndex(leafIdx)) continue;
        const Node& n = m_nodes[leafIdx];
        if (n.objectId.isIDValid()) saved.push_back(LeafTemp{n.objectId, n.bounds});
    }
    // swap into leaf temps and rebuild
    m_leafTemps.swap(saved);
    m_nodes.clear();
    m_objectToLeaf.clear();
    m_fatBounds.clear();
    m_dirtyLeaves.clear();
    m_deletedCount = 0;
    m_needsCompact = false;
    build(); // this will also rebuild fat bounds & object map
}

// ---------- Queries ----------
void BVH2D_LazyGC::queryPoint(const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const
{
    if (m_nodes.empty()) return;
    std::stack<int> st;
    st.push(0);
    while (!st.empty())
    {
        int idx = st.top();
        st.pop();
        if (!validNodeIndex(idx)) continue;
        auto&& n = m_nodes[idx];
        if (n.removed) {
            continue;
        }
        //if ((n.left < 0 && n.right < 0) || !n.bounds.contains(p)) continue;
        if (!n.bounds.contains(p)) continue;
        if (isLeaf(n))
        {
            if (n.objectId.isIDValid()) outIds.push_back(n.objectId);
        }
        else
        {
            if (n.right >= 0) st.push(n.right);
            if (n.left >= 0) st.push(n.left);
        }
    }
}

void BVH2D_LazyGC::queryBounds(const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const
{
    if (m_nodes.empty()) return;
    std::stack<int> st;
    st.push(0);

    static bool flag = true;

    //if (flag)
    //{
    //    printf("v2 queryBounds() AAA 01,m_nodes.size(): %lld\n", m_nodes.size());
    //    for (auto& n : m_nodes)
    //    {
    //        printf("v2 queryBounds() AAA 01, n.objectId: %s, n(l=%d,r=%d)\n", n.objectId.idToString().c_str(), n.left, n.right);
    //    }
    //}
    while (!st.empty())
    {
        int idx = st.top();
        st.pop();

        //if (flag)
        //{
        //    printf("v2 queryBounds() CCC,idx: %d, validNodeIndex(idx): %d\n", idx, validNodeIndex(idx));
        //}

        if (!validNodeIndex(idx)) continue;

        auto&& n = m_nodes[idx];
        //if (flag)
        //{
        //    printf("v2 queryBounds() CCC,idx: %d, n.objectId: %s, n(l=%d,r=%d)\n", idx, n.objectId.idToString().c_str(), n.left, n.right);
        //}
        //if (!n.bounds.intersects(b)) continue;

        if (n.removed)
        {
            continue;
        }
        //if ((n.left < 0 && n.right < 0) || !n.bounds.intersects(b)) {
        if (!n.bounds.intersects(b)) {
            continue;
        }

        if (isLeaf(n))
        {
            if (n.objectId.isIDValid() && n.bounds.intersects(b))
            {
                outIds.push_back(n.objectId);
            }
        }
        else
        {
            if (n.right >= 0) st.push(n.right);
            if (n.left >= 0) st.push(n.left);
        }
    }

    flag = false;
}


// recursive builder from temps (indices range [l,r))
int BVH2D_LazyGC::buildRecursiveFromLeaves(std::vector<int>& indices, int l, int r, int parent)
{
    int nodeIndex = (int)m_nodes.size();
    m_nodes.emplace_back();
    Node& node  = m_nodes.back();
    node.parent = parent;

    // compute bounds
    Math::Bounds b;
    b.toEmpty();
    for (int i = l; i < r; ++i) b.expand(m_leafTemps[indices[i]].bounds);
    node.bounds = b;

    int count = r - l;
    if (count == 1)
    {
        const LeafTemp& lt = m_leafTemps[indices[l]];
        node.objectId      = lt.objectId;
        node.left = node.right = -1;
        //printf("v2 buildRecursiveFromLeaves() B, indices[l]:%d, node.objectId: %s, node(l=%d, r=%d), nodeIndex: %d\n", indices[l], node.objectId.idToString().c_str(), node.left, node.right, nodeIndex);
        return nodeIndex;
    }

    auto ext  = node.bounds.extent();
    int  axis = (ext.y > ext.x) ? 1 : 0;

    // median by center
    float mid = 0.0f;
    for (int i = l; i < r; ++i)
    {
        auto c = m_leafTemps[indices[i]].bounds.center();
        mid += (axis == 0) ? c.x : c.y;
    }
    mid /= float(count);

    auto it       = std::partition(indices.begin() + l, indices.begin() + r,
                                   [&](int idx) {
                                 auto c = m_leafTemps[idx].bounds.center();
                                 return (axis == 0) ? (c.x < mid) : (c.y < mid);
                             });
    int  midIndex = int(it - indices.begin());
    if (midIndex == l || midIndex == r) midIndex = l + (count / 2);

    int leftIdx  = buildRecursiveFromLeaves(indices, l, midIndex, nodeIndex);
    int rightIdx = buildRecursiveFromLeaves(indices, midIndex, r, nodeIndex);

    node.left     = leftIdx;
    node.right    = rightIdx;
    node.objectId = Base::ID::INVALID_KEY;

    auto&& currNode = m_nodes[nodeIndex];
    currNode.left   = leftIdx;
    currNode.right  = rightIdx;
    //printf("v2 buildRecursiveFromLeaves() D, indices[l]:%d, node.objectId: %s, node(l=%d, r=%d), nodeIndex: %d\n", indices[l], node.objectId.idToString().c_str(), node.left, node.right, nodeIndex);
    node.bounds = Math::Bounds::Union(m_nodes[leftIdx].bounds, m_nodes[rightIdx].bounds);
    return nodeIndex;
}

void BVH2D_LazyGC::markAncestorsDirtyUpToRoot(int leafIdx)
{
    int cur = leafIdx;
    while (cur >= 0)
    {
        int p = m_nodes[cur].parent;
        if (p < 0) break;
        // recompute parent from children
        if (m_nodes[p].left >= 0 && m_nodes[p].right >= 0)
        {
            m_nodes[p].bounds = Math::Bounds::Union(m_nodes[m_nodes[p].left].bounds, m_nodes[m_nodes[p].right].bounds);
        }
        else if (m_nodes[p].left >= 0)
        {
            m_nodes[p].bounds = m_nodes[m_nodes[p].left].bounds;
        }
        else if (m_nodes[p].right >= 0)
        {
            m_nodes[p].bounds = m_nodes[m_nodes[p].right].bounds;
        }
        cur = p;
    }
}

// choose subtree root to rebuild for a leaf
int BVH2D_LazyGC::chooseSubtreeRootForLeaf(int leafIdx)
{
    int cur = leafIdx;
    while (true)
    {
        int parent = m_nodes[cur].parent;
        if (parent < 0) {
            break;
        }
        int leafCount = countLeavesUnderNode(parent);
        if (leafCount > m_subtreeLeafLimit) {
            cur = parent;
            break;
        }
        cur = parent;
    }
    return cur;
}

// count leaves under node (simple DFS)
int BVH2D_LazyGC::countLeavesUnderNode(int nodeIdx)
{
    int             cnt = 0;
    std::stack<int> st;
    st.push(nodeIdx);
    while (!st.empty())
    {
        int idx = st.top();
        st.pop();
        const Node& n = m_nodes[idx];
        if (isLeaf(n)) ++cnt;
        else
        {
            if (n.left >= 0) st.push(n.left);
            if (n.right >= 0) st.push(n.right);
        }
    }
    return cnt;
}

void BVH2D_LazyGC::rebuildSubtreeAtNode(int nodeIdx)
{
    if (!validNodeIndex(nodeIdx)) return;

    // collect leaves under nodeIdx
    std::vector<LeafTemp> leaves;
    std::stack<int>       st;
    st.push(nodeIdx);
    while (!st.empty())
    {
        int idx = st.top();
        st.pop();
        auto&& n = m_nodes[idx];
        if (isLeaf(n))
        {
            if (n.objectId.isIDValid()) {
                leaves.push_back(LeafTemp{n.objectId, n.bounds});
                n.removed = true;
                n.objectId = Base::ID::INVALID_KEY;
            }
        }
        else
        {
            if (n.left >= 0) st.push(n.left);
            if (n.right >= 0) st.push(n.right);
        }
    }
    if (leaves.empty()) return;

    // swap out current leafTemps temporarily
    auto saved  = std::move(m_leafTemps);
    m_leafTemps = std::move(leaves);

    // prepare indices
    std::vector<int> indices((int)m_leafTemps.size());
    for (int i = 0; i < (int)indices.size(); ++i) indices[i] = i;
    if (nodeIdx == 0)
    {
        m_nodes.clear();
    }

    int newRootIdx = buildRecursiveFromLeaves(indices, 0, (int)indices.size(), -1);

    printf("BVH2D_LazyGC::rebuildSubtreeAtNode(), m_nodes.size(): %lld\n", m_nodes.size());
    // restore previous leafTemps
    m_leafTemps = std::move(saved);

    int parent = m_nodes[nodeIdx].parent;
    if (parent >= 0)
    {
        Node& p = m_nodes[parent];
        if (p.left == nodeIdx) p.left = newRootIdx;
        else if (p.right == nodeIdx)
            p.right = newRootIdx;
        m_nodes[newRootIdx].parent = parent;
    }
    else
    {
        // nodeIdx is root: swap contents
        if (newRootIdx != nodeIdx)
        {
            std::swap(m_nodes[nodeIdx], m_nodes[newRootIdx]);
            // fix parent references of children
            if (m_nodes[nodeIdx].left >= 0) m_nodes[m_nodes[nodeIdx].left].parent = nodeIdx;
            if (m_nodes[nodeIdx].right >= 0) m_nodes[m_nodes[nodeIdx].right].parent = nodeIdx;
        }
    }
    // Note: old subtree remains unreachable; will be reclaimed by compactIfNeeded()
}

void BVH2D_LazyGC::refitAllNodes()
{
    if (m_nodes.empty()) return;

    // 先收集 preorder（根到叶），再反转为 postorder（子先父后）
    std::vector<int> order;
    order.reserve(m_nodes.size());
    std::stack<int> st;
    st.push(0);
    while (!st.empty())
    {
        int idx = st.top();
        st.pop();
        order.push_back(idx);
        const Node& n = m_nodes[idx];
        if (n.left >= 0) st.push(n.left);
        if (n.right >= 0) st.push(n.right);
    }

    // postorder: children first
    for (auto it = order.rbegin(); it != order.rend(); ++it)
    {
        Node& node = m_nodes[*it];

        if (isLeafNodeSlot(node))
        {
            if (node.objectId.isIDValid())
            {
                // 有效叶：bounds 已经存在（通常由更新函数设置）
                // (nothing to do)
            }
            else
            {
                // 已删除的叶槽：把 bounds 置为空（toEmpty），以便父节点正确合并
                node.bounds.toEmpty();
            }
        }
        else
        {
            // internal node: 合并孩子
            bool leftValid  = (node.left >= 0) && (isValidLeaf(m_nodes[node.left]) || !isLeafNodeSlot(m_nodes[node.left]));
            bool rightValid = (node.right >= 0) && (isValidLeaf(m_nodes[node.right]) || !isLeafNodeSlot(m_nodes[node.right]));

            if (leftValid && rightValid)
            {
                node.bounds = Math::Bounds::Union(m_nodes[node.left].bounds, m_nodes[node.right].bounds);
            }
            else if (leftValid)
            {
                node.bounds = m_nodes[node.left].bounds;
            }
            else if (rightValid)
            {
                node.bounds = m_nodes[node.right].bounds;
            }
            else
            {
                node.bounds.toEmpty();
            }
        }
    }
}



// full rebuild using current reachable leaves
void BVH2D_LazyGC::collectLeavesToTempsAndRebuild()
{
    std::vector<LeafTemp> saved;
    saved.reserve(m_objectToLeaf.size());
    for (const auto& kv : m_objectToLeaf)
    {
        int leafIdx = kv.second;
        if (!validNodeIndex(leafIdx)) continue;
        const Node& n = m_nodes[leafIdx];
        if (n.objectId.isIDValid()) saved.push_back(LeafTemp{n.objectId, n.bounds});
    }
    m_leafTemps.swap(saved);
    build();
}

void BVH2D_LazyGC::rebuildObjectMap()
{
    m_objectToLeaf.clear();
    for (int i = 0; i < (int)m_nodes.size(); ++i)
    {
        const Node& n = m_nodes[i];
        if (isLeaf(n) && n.objectId.isIDValid()) m_objectToLeaf[n.objectId] = i;
    }
}

}
}