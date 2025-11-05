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
    if (id == ID::INVALID_ID)
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.parent != ID::INVALID_ID;
}

bool EntityCompStorage::hasChildAt(uint32_t id) const
{
    if (ID::isInvalidID(id))
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.firstChild != ID::INVALID_ID;
}

Component::UnitTransform EntityCompStorage::getEntityLocalTransAt(uint32_t id)
{
    if (ID::isInvalidID(id))
        return Component::defaultTrans;

    auto&& et = entitiesPool[id];
    if (et.transformId == ID::INVALID_ID)
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
    if (ID::isInvalidID(id))
        return {};

    auto parentId = getEntityParentIdAt(id);
    // printf("getEntityParentGlobalMatAt(), id: %u, parentId: %u\n", id, parentId);
    return getEntityGlobalMatAt(parentId);
}
Math::Mat33 EntityCompStorage::getEntityGlobalMatAt(uint32_t id)
{
    if (ID::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return Math::Mat33::makeIdentity();
    }
    return entityGlobalMat33Map[id];
}

Math::Bounds EntityCompStorage::getEntityGlobalBoundsAt(uint32_t id)
{
    if (ID::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return {};
    }

    Math::Bounds tb;
    Component::defaultRect.mat33MapTo(entityGlobalMat33Map[id], tb);
    return tb;
}

uint32_t EntityCompStorage::getEntityParentIdAt(uint32_t id)
{
    if (id == ID::INVALID_ID)
        return ID::INVALID_ID;
    auto&& et = entitiesPool[id];
    // printf("getEntityParentIdAt() id: %u, et.hierarchyId: %u\n", id, et.hierarchyId);
    if (et.hierarchyId == ID::INVALID_ID)
        return ID::INVALID_ID;
    return hierarchiesPool[et.hierarchyId].parent;
}

Math::Vec2 EntityCompStorage::getEntityGlobalXYAt(uint32_t id)
{
    if (ID::isInvalidID(id))
        return {};
    return entityGlobalMat33Map[id].getXY();
}
void EntityCompStorage::setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (ID::isInvalidID(id))
        return;

    entityGlobalMat33Map[id].setXY(pv);
    auto&& et = entitiesPool[id];
    if (et.transformId == ID::INVALID_ID)
        return;

    auto wpv = entityGlobalMat33Map[id].getXY();

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
    if (ID::isInvalidID(id))
        return {};

    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}

void EntityCompStorage::setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (ID::isInvalidID(id))
        return;
    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    trans.pos()  = pv;
}

void EntityCompStorage::getIdsFromId(uint32_t etId, std::vector<ID::KeyUint64>& ids)
{
    if (ID::isInvalidID(etId))
        return;

    ids.push_back(ID::KeyUint64::make(etId, 0));

    for (auto child = hierarchiesPool[etId].firstChild;
         child != ID::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        getIdsFromId(child, ids);
    }
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(uint32_t id)
{
    if (ID::isInvalidID(id))
        return {};

    auto&& et = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{
    if (ID::isInvalidID(id))
        return;
    auto&& et                      = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}
Math::Mat33 EntityCompStorage::getEntityGlobalMat33At(uint32_t id)
{
    if (ID::isInvalidID(id))
        return {};

    return entityGlobalMat33Map[id];
}

void EntityCompStorage::checkIds(std::vector<ID::KeyUint64>& edis)
{
    if (edis.empty())
        return;

    std::vector<ID::KeyUint64> ids{};
    auto                       tot = edis.size();
    for (auto i = 0; i < tot; ++i)
    {
        auto&& et = entitiesPool[edis[i].protoId()];
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
    std::sort(edis.begin(), edis.end(), [&](ID::KeyUint64 a, ID::KeyUint64 b) {
        return hierarchyIndexMap[a] < hierarchyIndexMap[b];
        //return a < b;
    });
}


void EntityCompStorage::updateHierarchyInfo()
{
    uint32_t index = 0;
    traverseSortIndex(0, index);
    //traverseSortIndexAndBuildGlobalMat(0, index, {});
    traverseBuildGlobalMat(0, {});
}

void EntityCompStorage::traverseSortWithShadowEffect(const ID::KeyUint64& srcKey, uint32_t protoId, uint32_t& index)
{
    if (entitiesPool.isValid(protoId))
    {
        auto&& et = entitiesPool[protoId];
        if (ID::isValidID(et.shadingId))
        {
            auto&& shadingEt = shaderingEntitiesPool[et.shadingId];
            auto&& shdDesc   = shaderingDescPool[shadingEt.shadingDescId];
            if (shdDesc.flags > 0 && effectShadowIdMap.contains(shadingEt.shadingDescId))
            {
                auto&& effects = effectShadowIdMap[shadingEt.shadingDescId];
                for (auto ef : effects)
                {
                    auto&& key = ID::KeyUint64::makeWithEffectShadow(srcKey, ef);
                    printf("traverseSortWithShadowEffect(), key:%s index: %u\n", key.idToString().c_str(), index);
                    hierarchyIndexMap[key] = index++;
                }
            }
        }
    }
}


void EntityCompStorage::traverseSortIndexWithInstance(uint32_t iid, uint32_t protoId, uint32_t& index)
{
    auto&& key = ID::KeyUint64::make(protoId, iid);

    traverseSortWithShadowEffect(key, protoId, index);

    hierarchyIndexMap[key] = index++;

    auto&& et             = entitiesPool[protoId];
    auto   effectiveProto = ID::isValidID(et.prototypeId) ? et.prototypeId : protoId;

    for (auto child = hierarchiesPool[effectiveProto].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        auto& cet = entitiesPool[child];
        if (ID::isValidID(cet.prototypeId))
        {
            traverseSortIndexWithInstance(iid, cet.prototypeId, index);
        }
        else
        {
            traverseSortIndexWithInstance(iid, child, index);
        }
    }
}

void EntityCompStorage::traverseSortIndex(uint32_t protoId, uint32_t& index)
{

    auto&& key = ID::KeyUint64::make(protoId);
    traverseSortWithShadowEffect(key, protoId, index);
    hierarchyIndexMap[key] = index++;

    auto&& et              = entitiesPool[protoId];
    if (ID::isValidID(et.prototypeId))
    {
        auto iid = protoId;
        protoId  = et.prototypeId;

        for (auto child = hierarchiesPool[protoId].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            traverseSortIndexWithInstance(iid, child, index);
        }
        return;
    }

    for (auto child = hierarchiesPool[protoId].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseSortIndex(child, index);
    }
    /*
    printf("traverseSortIndex(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[ID::KeyUint64::make(etId)] = index++;

    for (auto child = hierarchiesPool[etId].firstChild; ID::isValidID(child); child = hierarchiesPool[child].next)
    {
        traverseSortIndex(child, index);
    }
    //*/
}

void EntityCompStorage::traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat)
{
    auto&& et = entitiesPool[etId];

    //printf("traverseSortIndexAndBuildGlobalMat(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[ID::KeyUint64::make(etId)] = index++;
    if (ID::isValidID(et.transformId))
    {
        auto& tr = transformsPool[et.transformId];
        //printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

        // get translation only
        auto&& parentTrans = parentMat.getXY();
        printf("    ins parentTrans pos(x=%f,y=%f)\n", parentTrans.x, parentTrans.y);

        auto&& worldMat = Math::Mat33::makeTranslate(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);

        entityGlobalMat33Map[etId] = worldMat;
        //worldMat.print();
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseSortIndexAndBuildGlobalMat(child, index, entityGlobalMat33Map[etId]);
    }
}

void EntityCompStorage::buildTopoOrderFromRoots(const std::vector<uint32_t>& roots)
{
    topoOrder.clear();
    topoIndex.clear();
    topoIndex.reserve(entitiesPool.capacity());

    uint32_t                      index = 0;
    std::function<void(uint32_t)> dfs   = [&](uint32_t id) {
        if (ID::isInvalidID(id)) return;
        topoOrder.push_back(id);
        topoIndex[id] = index++;
        for (uint32_t child = hierarchiesPool[id].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfs(child);
        }
    };
    for (auto r : roots) dfs(r);
}
void EntityCompStorage::traverseBuildGlobalMatInstance(uint32_t iid, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap)
{
    if (ID::isInvalidID(iid)) return;

    auto&&      et       = entitiesPool[iid];
    Math::Mat33 worldMat = parentMat;

    if (ID::isValidID(et.transformId))
    {
        auto&& tr = transformsPool[et.transformId];

        // compose: world = parentTranslation + local (translation-only from parent)
        // extract parent translation:
        auto&& parentTrans = parentMat.getXY();
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }

    auto&& key = ID::KeyUint64::make(iid, 0);
    // store in insMap (prototype node id = iid if called on prototype tree)
    insMap.map[key] = {key};

    // traverse prototype's children (note: when instancing, we traverse prototype hierarchy)
    for (auto child = hierarchiesPool[iid].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMatInstance(child, worldMat, insMap);
    }
}

void EntityCompStorage::traverseBuildGlobalMatPrototypeUnderInstance(uint32_t iid, uint32_t protoId, const Math::Mat33& instanceParentMat)
{
    Component::UnitInstanceMap map;
    map.iid     = iid;
    map.protoId = protoId;
    map.map.clear();

    // for prototype traversal, use the prototype hierarchy nodes
    // but we need to combine prototype-local transform + instanceParentMat
    std::function<void(uint32_t etIID, uint32_t protoId, const Math::Mat33&)> dfsEntity = [&](uint32_t etIID, uint32_t protoId, const Math::Mat33& parentMat) {
        Math::Mat33 worldMat = parentMat;
        // get prototype node's local transform if exists
        // prototype nodes are also stored in entityPool (we assume prototype entities have transforms)

        auto& protoEt = entitiesPool[protoId];
        if (ID::isValidID(protoEt.transformId))
        {
            auto&& tr = transformsPool[protoEt.transformId];

            auto&& parentTrans = parentMat.getXY();
            worldMat.identity();
            worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
            worldMat.setScaleXY(tr.sx, tr.sy);

            //printf("traverseBuildGlobalMatPrototypeUnderInstance(), A tr pos(x=%f,y=%f)\n", tr.x, tr.y);
            //printf("traverseBuildGlobalMatPrototypeUnderInstance(), B parentTrans pos(x=%f,y=%f)\n", parentTrans.x, parentTrans.y);
        }

        //auto&& pos = worldMat.getXY();
        //printf("traverseBuildGlobalMatPrototypeUnderInstance() protoId: %u, etIID: %u, C pos(x=%f,y=%f)\n", protoId, etIID, pos.x, pos.y);
        auto&& key                   = ID::KeyUint64::make(protoId, etIID);
        map.map[key]                 = {key};
        entityInsGlobalMat33Map[key] = worldMat;

        for (auto child = hierarchiesPool[protoId].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfsEntity(etIID, child, worldMat);
        }
    };

    auto&& key                   = ID::KeyUint64::make(protoId, iid);
    entityInsGlobalMat33Map[key] = instanceParentMat;

    for (auto child = hierarchiesPool[protoId].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        dfsEntity(iid, child, instanceParentMat);
    }
    // store into insStorage
    insStorage[iid] = std::move(map);
}
void EntityCompStorage::traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat)
{
    if (ID::isInvalidID(etId)) return;

    auto&&      et       = entitiesPool[etId];
    Math::Mat33 worldMat = parentMat;

    if (ID::isValidID(et.transformId))
    {
        auto&& tr          = transformsPool[et.transformId];
        auto&& parentTrans = parentMat.getXY();
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }

    entityGlobalMat33Map[etId] = worldMat;

    // if entity is instance of prototype, build instance map using prototype tree
    if (ID::isValidID(et.prototypeId))
    {
        //printf("EntityCompStorage::traverseBuildGlobalMat() has prototypeId: %u\n", et.prototypeId);
        if (ID::isValidID(hierarchiesPool[etId].firstChild))
        {
            printf("[Warning] entity %u is an instance, but has children in hierarchy ignored.\n", etId);
        }
        traverseBuildGlobalMatPrototypeUnderInstance(etId, et.prototypeId, worldMat);
        return;
    }

    // recurse children (entity children, not prototype children)
    for (auto child = hierarchiesPool[etId].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMat(child, worldMat);
    }
}

void EntityCompStorage::markSubtreeDirty(uint32_t rootId)
{
    if (ID::isInvalidID(rootId)) return;

    std::vector<uint32_t> stack{rootId};
    while (!stack.empty())
    {
        auto id = stack.back();
        stack.pop_back();
        entitiesPool[id].dirty = true;
        entityGlobalMat33Map.erase(id); // optional: clear old cached matrix
        for (auto c = hierarchiesPool[id].firstChild; ID::isValidID(c); c = hierarchiesPool[c].next)
            stack.push_back(c);
    }
}

// incremental updating
void EntityCompStorage::updateDirtySubtrees(const std::vector<uint32_t>& roots)
{
    auto&& idMat = Math::Mat33::makeIdentity();
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
                for (auto c = hierarchiesPool[id].firstChild; ID::isValidID(c); c = hierarchiesPool[c].next)
                    stack.push_back(c);
            }
        }
    }
}

void EntityCompStorage::collectAllEntitiesWithInstance(const ID::KeyUint64& etId, std::vector<ID::KeyUint64>& ids)
{
    ids.emplace_back(etId);

    auto protoId = etId.protoId();
    auto iid     = etId.iid();

    auto&    et             = entitiesPool[protoId];
    uint32_t effectiveProto = ID::isValidID(et.prototypeId) ? et.prototypeId : protoId;

    for (auto child = hierarchiesPool[effectiveProto].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        auto&    cet        = entitiesPool[child];
        uint32_t childProto = ID::isValidID(cet.prototypeId) ? cet.prototypeId : child;

        collectAllEntitiesWithInstance(ID::KeyUint64::make(childProto, iid), ids);
    }
}

void EntityCompStorage::collectAllEntities(const ID::KeyUint64& etId, std::vector<ID::KeyUint64>& ids)
{
    if (ID::isInvalidID(etId))
        return;


    ids.emplace_back(etId);
    auto protoId = etId.protoId();
    auto iid     = etId.iid();
    // 找到所有其他相关的实例
    for (auto&& insItem : insStorage)
    {
        auto&  ins = insItem.second;
        auto&& key = ID::KeyUint64::make(protoId, ins.iid);
        if (ins.map.contains(key))
        {
            collectAllEntitiesWithInstance(key, ids);
        }
    }

    if (iid > 0)
    {

        //printf("traverseBuildIds() has a new instance entity.\n");
        auto&    protoEntity    = entitiesPool[protoId];
        uint32_t effectiveProto = ID::isValidID(protoEntity.prototypeId) ? protoEntity.prototypeId : protoId;

        for (auto child = hierarchiesPool[effectiveProto].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            auto&    cet        = entitiesPool[child];
            uint32_t childProto = ID::isValidID(cet.prototypeId) ? cet.prototypeId : child;
            collectAllEntitiesWithInstance(ID::KeyUint64::make(childProto, iid), ids);
        }
        return;
    }
    auto& et = entitiesPool[protoId];
    if (ID::isValidID(et.prototypeId))
    {
        for (auto child = hierarchiesPool[et.prototypeId].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            auto&    cet        = entitiesPool[child];
            uint32_t childProto = ID::isValidID(cet.prototypeId) ? cet.prototypeId : child;
            collectAllEntitiesWithInstance(ID::KeyUint64::make(child, protoId), ids);
        }
        return;
    }
    for (auto child = hierarchiesPool[protoId].firstChild;
         ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        collectAllEntities(ID::KeyUint64::make(child), ids);
    }
}
void EntityCompStorage::updateAllInstanceGlobalMats(const ID::KeyUint64& etId)
{
    auto protoId = etId.protoId();
    if (ID::isInvalidID(protoId))
        return;
    if (entitiesPool.isInvalid(protoId))
    {
        return;
    }
    std::function<void(uint32_t iid, uint32_t protoId, const Math::Mat33&)> dfsEntity = [&](uint32_t iid, uint32_t protoId, const Math::Mat33& parentMat) {
        Math::Mat33 worldMat = parentMat;
        auto&       protoEnt = entitiesPool[protoId];
        if (ID::isValidID(protoEnt.transformId))
        {
            auto&  tr          = transformsPool[protoEnt.transformId];
            auto&& parentTrans = parentMat.getXY();
            worldMat.identity();
            worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
            worldMat.setScaleXY(tr.sx, tr.sy);
        }
        auto&& key = ID::KeyUint64::make(protoId, iid);

        entityInsGlobalMat33Map[key] = worldMat;

        for (auto child = hierarchiesPool[protoId].firstChild;
             ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfsEntity(iid, child, worldMat);
        }
    };

    for (auto&& insItem : insStorage)
    {
        auto&  ins = insItem.second;
        auto&& key = ID::KeyUint64::make(protoId, ins.iid);
        if (ins.map.contains(key))
        {
            auto parentProtoId = getEntityParentIdAt(protoId);
            if (ID::isValidID(parentProtoId))
            {
                auto&& parentKey = ID::KeyUint64::make(parentProtoId, ins.iid);
                if (entityInsGlobalMat33Map.contains(parentKey))
                {
                    auto&& parentWMat = entityInsGlobalMat33Map[parentKey];
                    dfsEntity(ins.iid, protoId, parentWMat);
                }
            }
        }
    }
}
} // namespace Voxol::Render