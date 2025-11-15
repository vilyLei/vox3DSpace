#ifndef VOXOL_SCENE_BVH2D_V1_H
#define VOXOL_SCENE_BVH2D_V1_H

#include "../Math/VxRect.h"
#include "../Base/IDDef.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

namespace Voxol::Scene
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
        Base::ID::KeyUint64 objectId;

        Math::Bounds bounds; 
        bool         dirty = false;
    };

    struct Node
    {
        Math::Bounds bounds;
        int          left      = -1;
        int          right     = -1;
        int          parent    = -1;
        int          itemIndex = -1; 
        bool         isLeaf() const { return itemIndex >= 0; }
    };

    BVH2D() = default;

    void addItem(const Base::ID::KeyUint64& objectId, const Math::Bounds& bounds);

    void build();

    bool updateItemBounds(int itemIndex, const Math::Bounds& newBounds);

    bool updateItemBoundsByObjectId(const Base::ID::KeyUint64& objectId, const Math::Bounds& newBounds);

    void updateDirty();

    void queryPoint(const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const;

    void queryBounds(const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const;

    const Item& getItem(int index) const { return m_items[index]; }
    Item&       getItem(int index) { return m_items[index]; }

    void partialRebuild();

    const Math::Bounds& getBoundsAt(const Base::ID::KeyUint64& objId)
    {
        auto i = m_objectIdToItem[objId];
        return m_items[i].bounds;
    }
    const int getBoundsCapacity() const
    {
        return static_cast<int>(m_items.size());
    }

private:

    int buildRecursive(int begin, int end);

    void queryPointRecursive(int nodeIndex, const Math::Vec2& p, std::vector<Base::ID::KeyUint64>& outIds) const;

    void queryBoundsRecursive(int nodeIndex, const Math::Bounds& b, std::vector<Base::ID::KeyUint64>& outIds) const;

    void rebuildObjectMap()
    {
        m_objectIdToItem.clear();
        for (int i = 0; i < (int)m_items.size(); ++i)
            m_objectIdToItem[m_items[i].objectId] = i;
    }

private:
    std::vector<Item>                                                                                                            m_items;
    std::vector<Node>                                                                                                            m_nodes;
    std::unordered_map<Base::ID::KeyUint64, uint32_t, Base::ID::IdTraits<Base::ID::KeyUint64>::Hasher, Base::ID::KeyUint64Equal> m_objectIdToItem;

    bool m_dirty = false;
};

} // namespace V1
} // namespace Voxol::Scene
#endif