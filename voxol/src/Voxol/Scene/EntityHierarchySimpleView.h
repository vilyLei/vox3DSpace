#ifndef VOXOL_SCENE_ENTITY_HIERARCHY_SIMPLE_VIEW_H
#define VOXOL_SCENE_ENTITY_HIERARCHY_SIMPLE_VIEW_H

#include "EntityCompStorage.h"
#include <deque>

namespace Voxol::Scene
{
using UpdateBVHCallbackType       = std::function<void(uint32_t id)>;
using UpdateTransformCallbackType = std::function<bool(const Math::Vec2& wPos, Component::UnitTransform& trans)>;
using PtApplyCallbackType         = std::function<bool(int index, const Math::Vec2& pv0, const Math::Vec2& pv1)>;

//class EntityHierarchySimpleView
//{
//public:
//    using SP = std::shared_ptr<EntityHierarchySimpleView>;
//    static SP make();


class EntityHierarchySimpleView
{
public:
    using SP = std::shared_ptr<EntityHierarchySimpleView>;
    using WP = std::weak_ptr<EntityHierarchySimpleView>;
    static SP make();

public:
    EntityHierarchySimpleView()  = default;
    ~EntityHierarchySimpleView() = default;

public:
    void initialize(const EntityCompStorage::SP& comp_storage);

    // 向上移动（与前一个兄弟节点交换位置）
    bool moveUp(uint32_t childId);
    // 向下移动（与后一个兄弟节点交换位置）
    bool moveDown(uint32_t childId);
    bool remove(uint32_t id);
    bool addToParent(uint32_t parentId, uint32_t childId);
    bool insertAfterToParent(uint32_t parentId, uint32_t afterId, uint32_t id);
    bool insertBeforeToParent(uint32_t parentId, uint32_t childId, uint32_t id);
    bool appendHeadToParent(uint32_t parentId, uint32_t id);
    bool appendTailToParent(uint32_t parentId, uint32_t id);

    // 遍历所有子树
    template <typename Func>
    void traverseDFS(uint32_t rootId, Func&& fn) const
    {
        auto&    hierarchies = compStorage->hierarchiesPool;
        uint32_t cur         = rootId;
        while (cur != Base::ID::INVALID_ID)
        {
            fn(cur);
            if (hierarchies[cur].firstChild != Base::ID::INVALID_ID)
            {
                traverseDFS(hierarchies[cur].firstChild, fn);
            }
            cur = hierarchies[cur].next;
        }
    }

    void destory();

private:
    EntityCompStorage::SP compStorage;
};

} // namespace Voxol::Scene
#endif