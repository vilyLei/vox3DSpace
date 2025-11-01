#include "EntityCompStorage.h"
#include <algorithm>
#include <functional>

namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}

bool EntityCompStorage::hasParentAt(uint32_t id) const
{
    if (id == Component::INVALID_ID)
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.parent != Component::INVALID_ID;
}

bool EntityCompStorage::hasChildAt(uint32_t id) const
{
    if (Component::isInvalidID(id))
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.firstChild != Component::INVALID_ID;
}

Component::UnitTransform EntityCompStorage::getEntityLocalTransAt(uint32_t id)
{
    if (Component::isInvalidID(id))
        return Component::defaultTrans;

    auto&& et = entitiesPool[id];
    if (et.transformId == Component::INVALID_ID)
        return Component::defaultTrans;

    auto&& trans = transformsPool[et.transformId];
    return trans;
}
Component::UnitTransform EntityCompStorage::getEntityParentLocalTransAt(uint32_t id)
{
    auto parentId = getEntityParentIdAt(id);
    return getEntityLocalTransAt(parentId);
}

bool EntityCompStorage::getEntityVisibleAt(uint32_t id)
{
    auto&& et = entitiesPool[id];
    return et.visible;
}

Math::Mat33 EntityCompStorage::getEntityParentGlobalMatAt(uint32_t id)
{
    if (Component::isInvalidID(id))
        return {};

    auto parentId = getEntityParentIdAt(id);
    // printf("getEntityParentGlobalMatAt(), id: %u, parentId: %u\n", id, parentId);
    return getEntityGlobalMatAt(parentId);
}
Math::Mat33 EntityCompStorage::getEntityGlobalMatAt(uint32_t id)
{
    if (Component::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return Math::Mat33::makeIdentity();
    }
    return entityGlobalMat33Map[id];
}

Math::Bounds EntityCompStorage::getEntityGlobalBoundsAt(uint32_t id)
{
    if (Component::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return {};
    }

    Math::Bounds tb;
    Component::defaultRect.mat33MapTo(entityGlobalMat33Map[id], tb);
    return tb;
}

uint32_t EntityCompStorage::getEntityParentIdAt(uint32_t id)
{
    if (id == Component::INVALID_ID)
        return Component::INVALID_ID;
    auto&& et = entitiesPool[id];
    // printf("getEntityParentIdAt() id: %u, et.hierarchyId: %u\n", id, et.hierarchyId);
    if (et.hierarchyId == Component::INVALID_ID)
        return Component::INVALID_ID;
    return hierarchiesPool[et.hierarchyId].parent;
}

Math::Vec2 EntityCompStorage::getEntityGlobalXYAt(uint32_t id)
{
    if (Component::isInvalidID(id))
        return {};
    return entityGlobalMat33Map[id].getXY();
}
void EntityCompStorage::setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (Component::isInvalidID(id))
        return;

    entityGlobalMat33Map[id].setXY(pv);
    auto&& et = entitiesPool[id];
    if (et.transformId == Component::INVALID_ID)
        return;

    auto wpv = entityGlobalMat33Map[id].getXY();
    // 获取去掉缩放值之后父级全局矩阵
    auto&&      mat = getEntityParentWorldMatWithoutScale(id);
    Math::Mat33 matInv;
    mat.inverseTo(matInv);
    auto&& lpv                           = matInv.mapPoint(wpv);
    transformsPool[et.transformId].pos() = lpv;
}

Math::Mat33 EntityCompStorage::getEntityWorldMatWithoutScale(uint32_t id)
{
    auto&& trans = getEntityLocalTransAt(id);
    // printf("getEntityWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
    auto&& temptMat = Math::Mat33::makeScale(trans.sx, trans.sy);

    Math::Mat33 temptMatInv;
    temptMat.inverseTo(temptMatInv);
    auto&& wmat = getEntityGlobalMatAt(id);
    wmat.append(temptMatInv);
    //parentMat.print();
    return wmat;
}
Math::Mat33 EntityCompStorage::getEntityParentWorldMatWithoutScale(uint32_t id)
{
    auto&& trans = getEntityParentLocalTransAt(id);
    // printf("getEntityParentWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
    auto&& temptMat = Math::Mat33::makeScale(trans.sx, trans.sy);

    Math::Mat33 temptMatInv;
    temptMat.inverseTo(temptMatInv);
    auto&& parentMat = getEntityParentGlobalMatAt(id);
    parentMat.append(temptMatInv);
    //parentMat.print();
    return parentMat;
}

Math::Vec2 EntityCompStorage::getEntityLocalXYAt(uint32_t id)
{
    if (Component::isInvalidID(id))
        return {};

    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}

void EntityCompStorage::setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (Component::isInvalidID(id))
        return;
    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    trans.pos()  = pv;
}

void EntityCompStorage::getIdsFromId(uint32_t etId, std::vector<uint32_t>& ids)
{
    if (Component::isInvalidID(etId))
        return;

    ids.push_back(etId);

    for (auto child = hierarchiesPool[etId].firstChild;
         child != Component::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        getIdsFromId(child, ids);
    }
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(uint32_t id)
{
    if (Component::isInvalidID(id))
        return {};

    auto&& et = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{
    if (Component::isInvalidID(id))
        return;
    auto&& et                      = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}
Math::Mat33 EntityCompStorage::getEntityGlobalMat33At(uint32_t id)
{
    if (Component::isInvalidID(id))
        return {};

    return entityGlobalMat33Map[id];
}

void EntityCompStorage::checkIds(std::vector<uint32_t>& edis)
{
    if (edis.empty())
    {
        return;
    }
    std::vector<uint32_t> ids{};
    auto                  tot = edis.size();
    for (auto i = 0; i < tot; ++i)
    {
        auto&& et = entitiesPool[edis[i]];
        if (!et.visible)
            continue;
        ids.push_back(edis[i]);
    }
    if (edis.size() != ids.size())
    {
        edis = ids;
    }
    if (edis.size() <= 1)
    {
        return;
    }
    std::sort(edis.begin(), edis.end(), [&](uint32_t a, uint32_t b) {
        return hierarchyIndexMap[a] < hierarchyIndexMap[b];
        //return a < b;
    });
}

void EntityCompStorage::updateHierarchyInfo()
{
    uint32_t index = 0;
    //traverseSortIndex(0, index);
    traverseSortIndexAndBuildGlobalMat(0, index, {});
}

void EntityCompStorage::traverseSortIndex(uint32_t etId, uint32_t& index)
{
    printf("traverseSortIndex(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[etId]  = index++;
    constexpr auto InvalidID = Component::INVALID_ID;
    for (auto child = hierarchiesPool[etId].firstChild; child != InvalidID; child = hierarchiesPool[child].next)
    {
        traverseSortIndex(child, index);
    }
}
void EntityCompStorage::traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat)
{
    constexpr auto InvalidID = Component::INVALID_ID;
    auto&&         et        = entitiesPool[etId];

    //printf("traverseSortIndexAndBuildGlobalMat(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[etId] = index++;
    if (et.transformId != InvalidID)
    {
        auto& tr = transformsPool[et.transformId];
        //printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

        // 仅传递平移：提取父矩阵的 translation
        auto&& parentTrans = parentMat.getXY();

        // 构造新的 world matrix：仅叠加平移
        Math::Mat33 worldMat;
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy); // 自身 scale 不受父级影响

        entityGlobalMat33Map[etId] = worldMat;
        //worldMat.print();
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         child != InvalidID;
         child = hierarchiesPool[child].next)
    {
        traverseSortIndexAndBuildGlobalMat(child, index, entityGlobalMat33Map[etId]);
    }
}

void EntityCompStorage::traverseBuildGlobalMatA(uint32_t etId, const Math::Mat33& parentMat)
{
    if (etId == Component::INVALID_ID)
    {
        return;
    }
    auto& entities = entitiesPool;

    auto& et = entities[etId];

    if (et.transformId != Component::INVALID_ID)
    {
        auto&& tr = transformsPool[et.transformId];


        auto&& parentTrans = parentMat.getXY();

        //printf("traverseBuildGlobalMat(), etId:%u, tr(x=%f,y=%f), ptr(x=%f,y=%f)\n", etId, tr.x, tr.y, parentTrans.x, parentTrans.y);

        auto&& worldMat = entityGlobalMat33Map[etId];
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }
    else
    {
        entityGlobalMat33Map[etId] = parentMat;
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         child != Component::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMatA(child, entityGlobalMat33Map[etId]);
    }
}

void EntityCompStorage::buildTopoOrderFromRoots(const std::vector<uint32_t>& roots)
{
    topoOrder.clear();
    topoIndex.clear();
    topoIndex.reserve(entitiesPool.capacity());

    uint32_t                      index = 0;
    std::function<void(uint32_t)> dfs   = [&](uint32_t id) {
        if (Component::isInvalidID(id)) return;
        topoOrder.push_back(id);
        topoIndex[id] = index++;
        for (uint32_t child = hierarchiesPool[id].firstChild;
             child != Component::INVALID_ID;
             child = hierarchiesPool[child].next)
        {
            dfs(child);
        }
    };

    for (auto r : roots) dfs(r);
}
void EntityCompStorage::traverseBuildGlobalMatInstance(uint32_t instanceRootId, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap)
{
    if (instanceRootId == Component::INVALID_ID) return;

    auto&&      et       = entitiesPool[instanceRootId];
    Math::Mat33 worldMat = parentMat;

    if (et.transformId != Component::INVALID_ID)
    {
        auto&& tr = transformsPool[et.transformId];

        // compose: world = parentTranslation + local (translation-only from parent)
        // extract parent translation:
        Math::Vec2 parentTrans = parentMat.getXY();
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }

    // store in insMap (prototype node id = instanceRootId if called on prototype tree)
    insMap.map[instanceRootId] = {instanceRootId, 0, worldMat};

    // traverse prototype's children (note: when instancing, we traverse prototype hierarchy)
    for (uint32_t child = hierarchiesPool[instanceRootId].firstChild;
         child != Component::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMatInstance(child, worldMat, insMap);
    }
}

void EntityCompStorage::traverseBuildGlobalMatPrototypeUnderInstance(uint32_t instanceEntityId, uint32_t prototypeRootId, const Math::Mat33& instanceParentMat)
{
    // create fresh InsNodeMap
    Component::UnitInstanceMap map;
    map.instanceEntityId = instanceEntityId;
    map.prototypeRootId  = prototypeRootId;
    map.map.clear();

    // for prototype traversal, use the prototype hierarchy nodes
    // but we need to combine prototype-local transform + instanceParentMat
    std::function<void(uint32_t, const Math::Mat33&)> dfsProto = [&](uint32_t protoNodeId, const Math::Mat33& parentMat) {
        Math::Mat33 worldMat = parentMat;
        // get prototype node's local transform if exists
        // prototype nodes are also stored in entityPool (we assume prototype entities have transforms)
        auto& protoEnt = entitiesPool[protoNodeId];
        if (protoEnt.transformId != Component::INVALID_ID)
        {
            auto&& tr = transformsPool[protoEnt.transformId];

            Math::Vec2 parentTrans = parentMat.getXY();
            worldMat.identity();
            worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
            worldMat.setScaleXY(tr.sx, tr.sy);
        }

        map.map[protoNodeId] = {protoNodeId, 0, worldMat};

        for (auto child = hierarchiesPool[protoNodeId].firstChild;
             child != Component::INVALID_ID;
             child = hierarchiesPool[child].next)
        {
            dfsProto(child, worldMat);
        }
    };

    // root prototype node(s) traversal
    dfsProto(prototypeRootId, instanceParentMat);

    // store into insStorage
    insStorage[instanceEntityId] = std::move(map);
}
void EntityCompStorage::traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat)
{
    if (etId == Component::INVALID_ID) return;

    auto&&      et       = entitiesPool[etId];
    Math::Mat33 worldMat = parentMat;

    if (et.transformId != Component::INVALID_ID)
    {
        auto&&     tr          = transformsPool[et.transformId];
        Math::Vec2 parentTrans = parentMat.getXY();
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }

    entityGlobalMat33Map[etId] = worldMat;

    // if entity is instance of prototype, build instance map using prototype tree
    if (et.prototypeId != Component::INVALID_ID)
    {
        traverseBuildGlobalMatPrototypeUnderInstance(etId, et.prototypeId, worldMat);
        // 当前entity为instance entity则当前的这个entity不可以再有其他子节点
        return;
    }

    // recurse children (entity children, not prototype children)
    for (auto child = hierarchiesPool[etId].firstChild;
         child != Component::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMat(child, worldMat);
    }
}

void EntityCompStorage::markSubtreeDirty(uint32_t rootId)
{
    if (rootId == Component::INVALID_ID) return;

    std::vector<uint32_t> stack{rootId};
    while (!stack.empty())
    {
        auto id = stack.back();
        stack.pop_back();
        entitiesPool[id].dirty = true;
        entityGlobalMat33Map.erase(id); // optional: clear old cached matrix
        for (auto c = hierarchiesPool[id].firstChild; c != Component::INVALID_ID; c = hierarchiesPool[c].next)
            stack.push_back(c);
    }
}

// incremental updating
void EntityCompStorage::updateDirtySubtrees(const std::vector<uint32_t>& roots)
{
    Math::Mat33 idMat;
    idMat.identity();
    // For each root, if dirty true, traverse
    for (auto r : roots)
    {
        // if root dirty or no worldMat cached -> rebuild subtree
        if (entitiesPool[r].dirty || entityGlobalMat33Map.find(r) == entityGlobalMat33Map.end())
        {
            traverseBuildGlobalMat(r, idMat);
            // clear dirty flags for subtree
            std::vector<uint32_t> stack{r};
            while (!stack.empty())
            {
                auto id = stack.back();
                stack.pop_back();
                entitiesPool[id].dirty = false;
                for (auto c = hierarchiesPool[id].firstChild; c != Component::INVALID_ID; c = hierarchiesPool[c].next)
                    stack.push_back(c);
            }
        }
    }
}
} // namespace Voxol::Render