#ifndef VOXOL_SCENE_BVH2D_V2_H
#define VOXOL_SCENE_BVH2D_V2_H

#include "../Math/VxRect.h"
#include "../Base/IDDef.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace Voxol::Scene
{
namespace V2
{

/*
Assumptions on Math::Bounds API used here:
- Bounds b; b.toEmpty();
- b.expand(otherBounds) or b.expand(point)
- b.contains(point)
- b.intersect(other)
- b.center().x / .y
- b.extent().x / .y
- static Math::Bounds::Union(a,b) to combine
- b.expanded(pad) returns a new Bounds expanded by pad in all directions
Adjust calls to your project's API if names differ.
*/

class BVH2D_LazyGC
{

public:
    using SP = std::shared_ptr<BVH2D_LazyGC>;
    using WP = std::weak_ptr<BVH2D_LazyGC>;
    using UP = std::unique_ptr<BVH2D_LazyGC>;

public:
    static BVH2D_LazyGC::SP make();

public:
    struct Node
    {
        Math::Bounds        bounds;
        int32_t             left     = -1;
        int32_t             right    = -1;
        int32_t             parent   = -1;
        Base::ID::KeyUint64 objectId = Base::ID::INVALID_KEY; // >=0 for leaf, -1 for internal or deleted slot
    };

    BVH2D_LazyGC()  = default;
    ~BVH2D_LazyGC() = default;

    // ---------- Configuration ----------
    void setFatPad(float pad) { m_fatPad = pad; }
    void setRebuildRatio(float r) { m_rebuildRatio = r; }
    void setDeleteRebuildRatio(float r) { m_deleteRebuildRatio = r; }
    void setSubtreeLeafLimit(int n) { m_subtreeLeafLimit = n; }
    void setCompactFactor(size_t f) { m_compactFactor = f; }

    // ---------- Add / Build ----------
    void addItem(const Base::ID::KeyUint64& objectId, const Math::Bounds& bounds);
    // Build entire BVH from m_leafTemps (clears previous nodes)
    void build();
    // ---------- Update (with fat bounds) ----------
    // Update bounds by object id; returns true if updated.
    bool updateItemBoundsByObjectId(const Base::ID::KeyUint64& objectId, const Math::Bounds& newBounds);
    // ---------- Remove (lazy) ----------
    // Mark object as removed. Removal is lazy; node remains in m_nodes until compact.
    // Returns true if removed.
    bool removeItemByObjectId(const Base::ID::KeyUint64& objectId);

    // --------- 分块 lazy GC（每帧处理有限工作量） ---------
    // 设计：把 leafSlots 分成块 (BLOCK_SIZE)，每帧处理 1..N 个块，合并空洞或移除已删除的槽。
    // 如果空洞太多或者总体节点数爆炸，转为 full rebuild。
    static constexpr int GC_BLOCK_SIZE = 4096; // 根据内存/对象密度调节
    // 记录哪些块有空洞（以便逐块压缩）
    std::unordered_set<int> m_dirtyBlocks; // 存放 blockIndex
    std::vector<int>        m_freeList;

    // 当删除一个 leaf 时，登记其 block：
    void registerDeletedSlotForBlock(int leafIdx)
    {
        int blockIndex = leafIdx / GC_BLOCK_SIZE;
        m_dirtyBlocks.insert(blockIndex);
    }

    // 每帧在 endFrameCompact 调用：处理至多 maxBlocksToProcess 个 block
    void endFrameCompact(size_t maxBlocksToProcess = 1);

    // 局部压缩：尝试紧缩单个 block（把该范围的有效叶收集到块前端并更新映射）
    // 这里的实现是一个“示例/概念实现”——实际可按你的容器结构优化以避免大量复制
    void compactBlock(int blockIndex);

    // 全量 compact: 收集所有 live leaves 到 leafTemps 并 full build（回收垃圾）
    void compactIfNeededFull();
    /*
    void compactIfNeededFull()
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
    //*/

    // ---------- Per-frame maintenance ----------
    // Process dirty leaves — partial rebuilds or full rebuild based on thresholds.
    void updateDirty()
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

    // Called at frame end to do lazy GC / compact heuristics.
    // Should be called once per frame (or less frequently) by host.
    // ---------- Compact: fully rebuild leaves from reachable leaves (reclaims deleted/garbage) ----------
    void compactIfNeeded()
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
    void queryPoint(const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const
    {
        if (m_nodes.empty()) return;
        std::stack<int> st;
        st.push(0);
        while (!st.empty())
        {
            int idx = st.top();
            st.pop();
            if (!validNodeIndex(idx)) continue;
            const Node& n = m_nodes[idx];
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

    void queryBounds(const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const
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
            const Node& n = m_nodes[idx];
            //if (flag)
            //{
            //    printf("v2 queryBounds() CCC,idx: %d, n.objectId: %s, n(l=%d,r=%d)\n", idx, n.objectId.idToString().c_str(), n.left, n.right);
            //}
            if (!n.bounds.intersects(b)) continue;
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

    // ---------- Utilities / debug ----------
    size_t              nodeCount() const { return m_nodes.size(); }
    size_t              leafCount() const { return m_objectToLeaf.size(); }
    size_t              deletedCount() const { return m_deletedCount; }
    const Math::Bounds& getBoundsAt(const Base::ID::KeyUint64& objId)
    {
        if (m_objectToLeaf.empty())
            return defaultBounds;
        auto i = m_objectToLeaf[objId];
        return m_nodes[i].bounds;
    }
    const int getBoundsCapacity() const
    {
        return static_cast<int>(m_nodes.size());
    }

private:
    Math::Bounds defaultBounds{0, 0, 1, 1};
    struct LeafTemp
    {
        Base::ID::KeyUint64 objectId = Base::ID::INVALID_KEY;
        Math::Bounds        bounds;
    };

    inline bool validNodeIndex(int idx) const
    {
        auto flag = idx >= 0 && idx < (int)m_nodes.size();
        return flag;
    }
    inline bool isLeaf(const Node& n) const
    {
        auto idFlag = n.objectId.isIDValid();
        auto flag   = idFlag && n.left == -1 && n.right == -1;
        return flag;
    }
    inline bool isLeafNodeSlot(const Node& n) const
    {
        return n.left == -1 && n.right == -1;
    }

    inline bool isValidLeaf(const Node& n) const
    {
        return isLeafNodeSlot(n) && n.objectId.isIDValid();
    }
    //inline bool validNodeIndex(int idx) const { return idx >= 0 && idx < (int)m_nodes.size(); }

    // recursive builder from temps (indices range [l,r))
    int buildRecursiveFromLeaves(std::vector<int>& indices, int l, int r, int parent)
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
        //printf("v2 buildRecursiveFromLeaves() D, indices[l]:%d, node.objectId: %s, node(l=%d, r=%d), nodeIndex: %d\n", indices[l], node.objectId.idToString().c_str(), node.left, node.right, nodeIndex);
        node.bounds = Math::Bounds::Union(m_nodes[leftIdx].bounds, m_nodes[rightIdx].bounds);
        return nodeIndex;
    }

    // mark ancestors bounds up to root (used for small moves within fat bounds)
    void markAncestorsDirtyUpToRoot(int leafIdx)
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
    int chooseSubtreeRootForLeaf(int leafIdx)
    {
        int cur = leafIdx;
        while (true)
        {
            int parent = m_nodes[cur].parent;
            if (parent < 0) return cur;
            int leafCount = countLeavesUnderNode(parent);
            if (leafCount > m_subtreeLeafLimit) return cur;
            cur = parent;
        }
    }

    // count leaves under node (simple DFS)
    int countLeavesUnderNode(int nodeIdx)
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

    // rebuild subtree rooted at nodeIdx by collecting leaves and appending a new subtree
    void rebuildSubtreeAtNode(int nodeIdx)
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
            const Node& n = m_nodes[idx];
            if (isLeaf(n))
            {
                if (n.objectId.isIDValid()) leaves.push_back(LeafTemp{n.objectId, n.bounds});
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

        int newRootIdx = buildRecursiveFromLeaves(indices, 0, (int)indices.size(), -1);

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
    // --------- 安全的 refitAllNodes() ----------
    // 将已删除叶视为“空”的 bounds（toEmpty），并在合并时根据子节点的有效性选择合并规则。
    void refitAllNodes()
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
    void collectLeavesToTempsAndRebuild()
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

    void rebuildObjectMap()
    {
        m_objectToLeaf.clear();
        for (int i = 0; i < (int)m_nodes.size(); ++i)
        {
            const Node& n = m_nodes[i];
            if (isLeaf(n) && n.objectId.isIDValid()) m_objectToLeaf[n.objectId] = i;
        }
    }

private:
    std::vector<LeafTemp>                          m_leafTemps;            // temps before build
    std::vector<Node>                              m_nodes;                // compact storage of tree
    Base::ID::keyUint64Unordered_map<int32_t>      m_objectToLeaf;         // objectId -> leaf node idx
    Base::ID::keyUint64Unordered_map<Math::Bounds> m_fatBounds;            // fat bounds per objectId
    std::unordered_set<int>                        m_dirtyLeaves;          // leaf node indices needing relocation
    size_t                                         m_deletedCount = 0;     // approx deleted count (for heuristics)
    bool                                           m_needsCompact = false; // mark that compaction may be beneficial
    bool                                           m_dirty        = false; // needs refit/partial rebuild

    // configs:
    float  m_fatPad             = 2.0f;
    float  m_rebuildRatio       = 0.15f;
    float  m_deleteRebuildRatio = 0.20f;
    int    m_subtreeLeafLimit   = 16;
    size_t m_compactFactor      = 6;
};

using BVH2D = BVH2D_LazyGC;

} // namespace V2
} // namespace Voxol::Scene
#endif