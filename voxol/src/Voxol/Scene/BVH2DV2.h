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
        bool                removed  = false;
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

    // ---------- Per-frame maintenance ----------
    // Process dirty leaves — partial rebuilds or full rebuild based on thresholds.
    void updateDirty();

    // Called at frame end to do lazy GC / compact heuristics.
    // Should be called once per frame (or less frequently) by host.
    // ---------- Compact: fully rebuild leaves from reachable leaves (reclaims deleted/garbage) ----------
    void compactIfNeeded();

    // ---------- Queries ----------
    void queryPoint(const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const;
    void queryBounds(const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const;

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

    // recursive builder from temps (indices range [l,r))
    int buildRecursiveFromLeaves(std::vector<int>& indices, int l, int r, int parent);

    // mark ancestors bounds up to root (used for small moves within fat bounds)
    void markAncestorsDirtyUpToRoot(int leafIdx);
    // choose subtree root to rebuild for a leaf
    int chooseSubtreeRootForLeaf(int leafIdx);
    // count leaves under node (simple DFS)
    int countLeavesUnderNode(int nodeIdx);
    int countValidLeavesUnderNode(int nodeIdx) const;

    // rebuild subtree rooted at nodeIdx by collecting leaves and appending a new subtree
    void rebuildSubtreeAtNode(int nodeIdx);

    // --------- 安全的 refitAllNodes() ----------
    // 将已删除叶视为“空”的 bounds（toEmpty），并在合并时根据子节点的有效性选择合并规则。
    void refitAllNodes();
    // full rebuild using current reachable leaves
    void collectLeavesToTempsAndRebuild();
    void rebuildObjectMap();

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