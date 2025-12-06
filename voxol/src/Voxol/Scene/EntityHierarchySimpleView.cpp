#include "EntityHierarchySimpleView.h"
#include "../Math/MathDef.h"
#include "../Math/Random.h"
#include "../Math/AngleInterpolator.h"

namespace Voxol::Scene
{
EntityHierarchySimpleView::SP EntityHierarchySimpleView::make()
{
    auto sp = std::make_shared<EntityHierarchySimpleView>();
    return sp;
}

void EntityHierarchySimpleView::initialize(const EntityCompStorage::SP& comp_storage)
{
    compStorage = comp_storage;
}
void EntityHierarchySimpleView::destory()
{
    compStorage     = nullptr;
}



// 向上移动（与前一个兄弟节点交换位置）
bool EntityHierarchySimpleView::moveUp(uint32_t childId)
{
    if (Base::ID::isInvalidID(childId)) return false;

    auto&    hierarchies = compStorage->hierarchiesPool;
    uint32_t parentId    = hierarchies[childId].parent;

    if (Base::ID::isInvalidID(parentId) || hierarchies[parentId].firstChild == childId)
    {
        // 已经是根节点或者第一个子节点，无法再向上移动
        return false;
    }

    // 1. 找到 childId 的前驱节点 prevId
    uint32_t prevId = hierarchies[parentId].firstChild;
    while (prevId != Base::ID::INVALID_ID && hierarchies[prevId].next != childId)
    {
        prevId = hierarchies[prevId].next;
    }

    if (Base::ID::isInvalidID(prevId))
    {
        // 结构异常，找不到前驱
        return false;
    }

    // 2. 将 childId 从当前位置移除
    remove(childId); // remove会断开childId的parent和next连接，并修复链表

    // 3. 将 childId 插入到 prevId 之前。
    // insertBeforeToParent 内部会调用 insertAfterToParent(parentId, prev of prevId, childId)
    return insertBeforeToParent(parentId, prevId, childId);
}

// 向下移动（与后一个兄弟节点交换位置）
bool EntityHierarchySimpleView::moveDown(uint32_t childId)
{
    if (Base::ID::isInvalidID(childId)) return false;

    auto&    hierarchies = compStorage->hierarchiesPool;
    uint32_t parentId    = hierarchies[childId].parent;

    if (Base::ID::isInvalidID(parentId))
    {
        // 根节点无法移动
        return false;
    }

    uint32_t nextId = hierarchies[childId].next;

    if (Base::ID::isInvalidID(nextId))
    {
        // 已经是最后一个子节点，无法再向下移动
        return false;
    }

    // 1. 找到 childId 的前驱节点 prevId（需要从头查找）
    uint32_t prevId = Base::ID::INVALID_ID;
    if (hierarchies[parentId].firstChild != childId)
    {
        prevId = hierarchies[parentId].firstChild;
        while (prevId != Base::ID::INVALID_ID && hierarchies[prevId].next != childId)
        {
            prevId = hierarchies[prevId].next;
        }
    }

    // 2. 将 childId 从当前位置移除
    remove(childId);

    // 3. 将 childId 插入到 nextId 之后
    return insertAfterToParent(parentId, nextId, childId);
}
bool EntityHierarchySimpleView::remove(uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return false;

    auto& hierarchies = compStorage->hierarchiesPool;

    auto&    hier   = hierarchies[id];
    uint32_t parent = hier.parent;
    if (Base::ID::isInvalidID(parent))
        return false;

    auto& parentHier = hierarchies[parent];

    // 如果 id 是第一个 child
    if (parentHier.firstChild == id)
    {
        parentHier.firstChild = hier.next;
    }
    else
    {
        // 找前驱
        uint32_t prev = parentHier.firstChild;
        while (prev != Base::ID::INVALID_ID && hierarchies[prev].next != id)
            prev = hierarchies[prev].next;

        if (prev != Base::ID::INVALID_ID)
            hierarchies[prev].next = hier.next;
    }

    hier.parent = Base::ID::INVALID_ID;
    hier.next   = Base::ID::INVALID_ID;

    return true;
}
bool EntityHierarchySimpleView::addToParent(uint32_t parentId, uint32_t childId)
{
    if (Base::ID::isInvalidID(parentId) || Base::ID::isInvalidID(childId))
        return false;

    remove(childId); // 先断开旧关系

    auto& hierarchies           = compStorage->hierarchiesPool;
    hierarchies[childId].parent = parentId;

    uint32_t& first = hierarchies[parentId].firstChild;
    if (first == Base::ID::INVALID_ID)
    {
        first = childId;
        return true;
    }

    // 找尾巴
    uint32_t cur = first;
    while (hierarchies[cur].next != Base::ID::INVALID_ID)
    {
        cur = hierarchies[cur].next;
    }

    hierarchies[cur].next = childId;
    return true;
}


bool EntityHierarchySimpleView::insertAfterToParent(uint32_t parentId, uint32_t afterId, uint32_t id)
{
    if (Base::ID::isInvalidID(parentId) || Base::ID::isInvalidID(id))
        return false;

    auto& hierarchies = compStorage->hierarchiesPool;

    // 如果 id 已经在某父节点下 → 先 detach
    if (Base::ID::isValidID(hierarchies[id].parent))
        remove(id);

    auto& parentHier = hierarchies[parentId];
    auto& hier       = hierarchies[id];

    if (afterId == Base::ID::INVALID_ID)
    {
        // 插到头部
        hier.next             = parentHier.firstChild;
        parentHier.firstChild = id;
    }
    else
    {
        auto& afterHier = hierarchies[afterId];
        hier.next       = afterHier.next;
        afterHier.next  = id;
    }

    hier.parent = parentId;
    return true;
}
bool EntityHierarchySimpleView::insertBeforeToParent(uint32_t parentId, uint32_t childId, uint32_t id)
{
    if (Base::ID::isInvalidID(parentId))
        return false;

    auto& hierarchies = compStorage->hierarchiesPool;

    auto& parentHier = hierarchies[parentId];

    // childId 是第一个 child
    if (parentHier.firstChild == childId)
        return insertAfterToParent(parentId, Base::ID::INVALID_ID, id);

    // 找 childId 的前驱
    uint32_t prev = parentHier.firstChild;
    while (prev != Base::ID::INVALID_ID && hierarchies[prev].next != childId)
        prev = hierarchies[prev].next;

    return insertAfterToParent(parentId, prev, id);
}
bool EntityHierarchySimpleView::appendHeadToParent(uint32_t parentId, uint32_t id)
{
    return insertAfterToParent(parentId, Base::ID::INVALID_ID, id);
}
bool EntityHierarchySimpleView::appendTailToParent(uint32_t parentId, uint32_t id)
{
    if (Base::ID::isInvalidID(parentId))
        return false;

    auto& hierarchies = compStorage->hierarchiesPool;

    auto& parentHier = hierarchies[parentId];

    uint32_t last = parentHier.firstChild;
    if (Base::ID::isInvalidID(last))
        return insertAfterToParent(parentId, Base::ID::INVALID_ID, id);

    while (hierarchies[last].next != Base::ID::INVALID_ID)
        last = hierarchies[last].next;

    return insertAfterToParent(parentId, last, id);
}

} // namespace Voxol::Scene