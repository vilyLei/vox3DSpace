#ifndef VOXOL_RENDER_BVH2D_V1_H
#define VOXOL_RENDER_BVH2D_V1_H

#include "../Math/VxRect.h"
#include "CompBaseDef.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Voxol::Render
{
namespace V1
{

class BVH2D
{
public:
    using SP = std::shared_ptr<BVH2D>;
    using WP = std::weak_ptr<BVH2D>;
    using UP = std::unique_ptr<BVH2D>;

public:
    static BVH2D::SP make();

public:
    struct Item
    {
        Base::KeyUint64 objectId;  // entity id

        Math::Bounds bounds;        // world-space bounds
        bool         dirty = false; // 标记是否需要 refit
    };

    struct Node
    {
        Math::Bounds bounds;
        int          left      = -1;
        int          right     = -1;
        int          parent    = -1;
        int          itemIndex = -1; // leaf节点对应的item索引
        bool         isLeaf() const { return itemIndex >= 0; }
    };

    BVH2D() = default;

    // -----------------------------
    // 添加 Item
    // -----------------------------
    void addItem(const Base::KeyUint64& objectId, const Math::Bounds& bounds);

    // -----------------------------
    // 完整构建BVH
    // -----------------------------
    void build();
    // -----------------------------
    // 更新单个对象的包围盒 (通过 index)
    // -----------------------------
    bool updateItemBounds(int itemIndex, const Math::Bounds& newBounds);

    // -----------------------------
    // 更新单个对象的包围盒 (通过 objectId)
    // -----------------------------
    bool updateItemBoundsByObjectId(const Base::KeyUint64& objectId, const Math::Bounds& newBounds);
    // -----------------------------
    // 更新所有脏节点 (Refit)
    // -----------------------------
    void updateDirty();

    // -----------------------------
    // 点查询
    // -----------------------------
    void queryPoint(const Math::Vec2& p, std::vector<Base::KeyUint64>& outIds) const;

    // -----------------------------
    // 范围查询
    // -----------------------------
    void queryBounds(const Math::Bounds& b, std::vector<Base::KeyUint64>& outIds) const;

    // -----------------------------
    // 获取Item引用
    // -----------------------------
    const Item& getItem(int index) const { return m_items[index]; }
    Item&       getItem(int index) { return m_items[index]; }

    // -----------------------------
    // 部分重建 (可选)
    // -----------------------------
    void partialRebuild();

    const Math::Bounds& getBoundsAt(const Base::KeyUint64& objId)
    {
        auto i = m_objectIdToItem[objId];
        return m_items[i].bounds;
    }
    const int getBoundsCapacity() const
    {
        return static_cast<int>(m_items.size());
    }

private:
    // -----------------------------
    // 递归构建
    // -----------------------------
    int buildRecursive(int begin, int end);

    // -----------------------------
    // 点查询递归
    // -----------------------------
    void queryPointRecursive(int nodeIndex, const Math::Vec2& p, std::vector<Base::KeyUint64>& outIds) const;

    // -----------------------------
    // 范围查询递归
    // -----------------------------
    void queryBoundsRecursive(int nodeIndex, const Math::Bounds& b, std::vector<Base::KeyUint64>& outIds) const;

    // -----------------------------
    // 同步 objectId → itemIndex 映射
    // -----------------------------
    void rebuildObjectMap()
    {
        m_objectIdToItem.clear();
        for (int i = 0; i < (int)m_items.size(); ++i)
            m_objectIdToItem[m_items[i].objectId] = i;
    }

private:
    std::vector<Item>                      m_items;
    std::vector<Node>                      m_nodes;
    std::unordered_map<Base::KeyUint64, uint32_t, Base::KeyUint64Hasher, Base::KeyUint64Equal> m_objectIdToItem;
    std::unordered_map<Base::KeyUint64, uint32_t, Base::KeyUint64Hasher,Base::KeyUint64Equal> m_objectIdToItemMap;

    bool m_dirty = false;
};

} // namespace V1
} // namespace Voxol::Render
#endif