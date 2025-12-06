#include "EntityCompStorage.h"
#include <algorithm>
#include <functional>

namespace Voxol::Scene
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}
uint32_t EntityCompStorage::createEntity(uint32_t color, const Math::Vec2& size)
{
    entityIdMax++;

    uint32_t newId = entityIdMax;

    auto&& shdDesc = shaderingDescPool[newId];
    auto&& model   = modelsPool[newId];
    model.toDrawRect();
    shdDesc.color       = color;
    auto&& shdEt        = shaderingEntitiesPool[newId];
    shdEt.id            = newId;
    shdEt.shadingDescId = newId;
    transformsPool[newId].scale() = size;

    auto&& entity    = entitiesPool[newId];
    entity.id        = newId;
    entity.shadingId = newId;
    entity.transformId = newId;
    entity.modelId     = newId;
    entity.hierarchyId = newId;

    auto&& hier     = hierarchiesPool[newId];
    hier.parent     = Base::ID::INVALID_ID;
    hier.firstChild = Base::ID::INVALID_ID;
    hier.next       = Base::ID::INVALID_ID;

    return newId;
}
uint32_t EntityCompStorage::copyAndppendEntityFromId(uint32_t id)
{
    if (Base::ID::isInvalidID(id) || id < 1)
        return Base::ID::INVALID_ID;

    entityIdMax++;

    uint32_t newId = entityIdMax;

    shaderingDescPool[newId] = shaderingDescPool[id];

    shaderingEntitiesPool[newId]               = shaderingEntitiesPool[id];
    shaderingEntitiesPool[newId].shadingDescId = newId;
    hierarchiesPool[newId]                     = hierarchiesPool[id];
    transformsPool[newId]                      = transformsPool[id];
    modelsPool[newId]                          = modelsPool[id];

    if (transformPivotMap.contains(id))
    {
        transformPivotMap[newId] = transformPivotMap[id];
    }
    if (textureMap.contains(id))
    {
        textureMap[newId] = textureMap[id];
    }

    auto entity = entitiesPool[id];
    entity.id         = newId;
    entity.shadingId  = newId;
    entity.transformId  = newId;
    entity.modelId      = newId;
    entity.hierarchyId  = newId;
    entitiesPool[newId] = entity;

    hierarchiesPool[newId].parent = hierarchiesPool[id].parent;

    hierarchiesPool[newId].firstChild = Base::ID::INVALID_ID;

    uint32_t lastSibling = id;
    while (hierarchiesPool[lastSibling].next != Base::ID::INVALID_ID)
    {
        lastSibling = hierarchiesPool[lastSibling].next;
    }

    // append to tail
    hierarchiesPool[lastSibling].next = newId;
    hierarchiesPool[newId].next       = Base::ID::INVALID_ID;

    return newId;
}

bool EntityCompStorage::hasParentAt(uint32_t id) const
{
    if (Base::ID::isInvalidID(id))
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.parent != Base::ID::INVALID_ID;
}

bool EntityCompStorage::hasChildAt(uint32_t id) const
{
    if (Base::ID::isInvalidID(id))
        return false;
    auto&& et   = entitiesPool[id];
    auto&& hier = hierarchiesPool[et.hierarchyId];
    return hier.firstChild != Base::ID::INVALID_ID;
}

Component::UnitTransform EntityCompStorage::getEntityLocalTransAt(uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return Component::defaultTrans;

    auto&& et = entitiesPool[id];
    if (et.transformId == Base::ID::INVALID_ID)
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
    if (Base::ID::isInvalidID(id))
        return {};

    auto parentId = getEntityParentIdAt(id);
    // printf("getEntityParentGlobalMatAt(), id: %u, parentId: %u\n", id, parentId);
    return getEntityGlobalMat33At(parentId);
}

Math::Bounds EntityCompStorage::getEntityGlobalBoundsAt(uint32_t id)
{
    if (Base::ID::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return {};
    }

    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];

    Math::Bounds srcBounds{0, 0, trans.sx, trans.sy};
    if (transformPivotMap.contains(et.id))
    {
        auto&& sv = transformPivotMap[et.id];
        srcBounds.setXYWH(-sv.x * trans.sx, -sv.y * trans.sy, trans.sx, trans.sy);
    }
    Math::Bounds tb;
    srcBounds.mat33MapTo(entityGlobalMat33Map[id], tb);
    return tb;
}
Math::Bounds EntityCompStorage::getEntityGlobalBoundsAt(const Base::ID::KeyUint64& id)
{

    if (id.isIDInvalid())
        return {};

    auto&&       et    = entitiesPool[id.protoId()];
    auto&&       trans = transformsPool[et.transformId];
    Math::Bounds tb;
    Math::Bounds srcBounds{0, 0, trans.sx, trans.sy};

    if (transformPivotMap.contains(et.id))
    {
        auto&& sv = transformPivotMap[et.id];
        srcBounds.setXYWH(-sv.x * trans.sx, -sv.y * trans.sy, trans.sx, trans.sy);
    }

    srcBounds.mat33MapTo(getEntityGlobalMat33At(id), tb);
    return tb;
}

Math::Bounds EntityCompStorage::getEntityLocalBoundsAt(uint32_t id)
{

    if (Base::ID::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return {};
    }
    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.transformId))
        return {};

    auto&& trans = transformsPool[et.transformId];
    if (transformPivotMap.contains(et.id))
    {
        auto&& sv = transformPivotMap[et.id];
        Math::Bounds bv;
        bv.setXYWH(-sv.x * trans.sx, -sv.y * trans.sy, trans.sx, trans.sy);
        return bv;
    }
    return {0, 0, trans.sx, trans.sy};
}

Math::Bounds EntityCompStorage::getEntityLocalBoundsAt(const Base::ID::KeyUint64& id)
{

    if (id.isIDInvalid())
        return {};

    auto&& et = entitiesPool[id.protoId()];
    if (Base::ID::isInvalidID(et.transformId))
        return {};

    auto&& trans = transformsPool[et.transformId];
    if (transformPivotMap.contains(et.id))
    {
        auto&&       sv = transformPivotMap[et.id];
        Math::Bounds bv;
        bv.setXYWH(-sv.x * trans.sx, -sv.y * trans.sy, trans.sx, trans.sy);
        return bv;
    }
    return {0, 0, trans.sx, trans.sy};
}

Math::Mat33 EntityCompStorage::getEntityLocalMatrixAt(uint32_t id)
{

    if (Base::ID::isInvalidID(id) || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return Math::Mat33::makeIdentity();
    }
    auto&& et    = entitiesPool[id];
    if (Base::ID::isInvalidID(et.transformId))
        return Math::Mat33::makeIdentity();

    auto&& trans = transformsPool[et.transformId];
    Math::Mat33 mat;
    if (transformPivotMap.contains(et.id))
    {
        auto&& tv = transformPivotMap[et.id];
        mat.setXY(-tv.x * trans.sx, -tv.y * trans.sy);
    }
    mat.setScaleXY(trans.sx, trans.sy);
    return mat;
}

Math::Mat33 EntityCompStorage::getEntityLocalMatrixAt(const Base::ID::KeyUint64& id)
{

    if (id.isIDInvalid())
        return {};

    auto&& et = entitiesPool[id.protoId()];

    if (Base::ID::isInvalidID(et.transformId))
        return Math::Mat33::makeIdentity();

    auto&& trans = transformsPool[et.transformId];
    Math::Mat33 mat;
    if (transformPivotMap.contains(et.id))
    {
        auto&& tv = transformPivotMap[et.id];
        mat.setXY(-tv.x * trans.sx, -tv.y * trans.sy);
    }
    mat.setScaleXY(trans.sx, trans.sy);
    return mat;
}
uint32_t EntityCompStorage::getEntityParentIdAt(uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return Base::ID::INVALID_ID;

    auto&& et = entitiesPool[id];
    // printf("getEntityParentIdAt() id: %u, et.hierarchyId: %u\n", id, et.hierarchyId);
    if (et.hierarchyId == Base::ID::INVALID_ID)
        return Base::ID::INVALID_ID;
    return hierarchiesPool[et.hierarchyId].parent;
}

Math::Vec2 EntityCompStorage::getEntityGlobalXYAt(uint32_t id) const
{
    if (Base::ID::isInvalidID(id))
        return {};
    if (!entityGlobalMat33Map.contains(id))
        return {};
    auto&& mat = entityGlobalMat33Map.at(id);
    return mat.getXY();
}
void EntityCompStorage::setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return;

    entityGlobalMat33Map[id].setXY(pv);
    auto&& et = entitiesPool[id];
    if (et.transformId == Base::ID::INVALID_ID)
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
    //auto&& trans = getEntityLocalTransAt(id);
    //// printf("getEntityWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
    //auto&& temptMat = Math::Mat33::makeScale(trans.sx, trans.sy);
    //Math::Mat33 temptMatInv;
    //temptMat.inverseTo(temptMatInv);
    auto&& wmat = getEntityGlobalMat33At(id);
    //wmat.append(temptMatInv);
    //parentMat.print();
    return wmat;
}
Math::Mat33 EntityCompStorage::getEntityParentWorldMatWithoutScale(uint32_t id)
{
    //auto&& trans = getEntityParentLocalTransAt(id);
    // printf("getEntityParentWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
    //auto&& temptMat = Math::Mat33::makeScale(trans.sx, trans.sy);
    //Math::Mat33 temptMatInv;
    //temptMat.inverseTo(temptMatInv);
    auto&& parentMat = getEntityParentGlobalMatAt(id);
    //parentMat.append(temptMatInv);
    //parentMat.print();
    return parentMat;
}

Math::Vec2 EntityCompStorage::getEntityLocalXYAt(uint32_t id) const
{
    if (Base::ID::isInvalidID(id))
        return {};

    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.transformId))
        return {};

    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}

void EntityCompStorage::setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return;
    auto&& et = entitiesPool[id];

    if (Base::ID::isInvalidID(et.transformId))
        return;
    auto&& trans = transformsPool[et.transformId];
    trans.pos()  = pv;

    auto&& parentMat   = getEntityParentGlobalMatAt(id);
    auto&& parentTrans = parentMat.getXY();
    auto&& worldMat    = Math::Mat33::makeTranslate(parentTrans.x + trans.x, parentTrans.y + trans.y);
    worldMat.setScaleXY(trans.sx, trans.sy);
    entityGlobalMat33Map[id] = worldMat;
}



float EntityCompStorage::getEntityRotationAt(uint32_t id) const
{
    if (Base::ID::isInvalidID(id))
        return {};

    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.transformId))
        return {};

    auto&& trans = transformsPool[et.transformId];
    return trans.rotation;
}

void EntityCompStorage::setEntityRotationAt(float rad, uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return;
    auto&& et = entitiesPool[id];

    if (Base::ID::isInvalidID(et.transformId))
        return;
    auto&& trans = transformsPool[et.transformId];
    trans.rotation = rad;
}

void EntityCompStorage::setEntityColorAt(const Colour::Component::Color& color, uint32_t id)
{

    if (Base::ID::isInvalidID(id))
        return;

    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.shadingId))
        return;

    auto&& shadingEt = shaderingEntitiesPool[et.shadingId];

    if (Base::ID::isInvalidID(shadingEt.shadingDescId))
        return;

    auto&& shdDesc = shaderingDescPool[shadingEt.shadingDescId];
    shdDesc.color  = color;
}

Colour::Component::Color EntityCompStorage::getEntityColorAt(uint32_t id) const
{

    Colour::Component::Color color;
    if (Base::ID::isInvalidID(id))
        return color;

    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.shadingId))
        return color;

    auto&& shadingEt = shaderingEntitiesPool[et.shadingId];

    if (Base::ID::isInvalidID(shadingEt.shadingDescId))
        return color;

    auto&& shdDesc = shaderingDescPool[shadingEt.shadingDescId];
    return shdDesc.color;
}

void EntityCompStorage::getIdsFromId(uint32_t etId, std::vector<Base::ID::KeyUint64>& ids)
{
    if (Base::ID::isInvalidID(etId))
        return;

    ids.push_back(Base::ID::KeyUint64::make(etId));

    for (auto child = hierarchiesPool[etId].firstChild;
         child != Base::ID::INVALID_ID;
         child = hierarchiesPool[child].next)
    {
        getIdsFromId(child, ids);
    }
}

void EntityCompStorage::setEntityVisibleAt(bool v, uint32_t id) {

    if (Base::ID::isInvalidID(id))
        return;

    auto&& et = entitiesPool[id];
    et.visible = v;
}
bool EntityCompStorage::getEntityVisibleAt(uint32_t id) const{

    if (Base::ID::isInvalidID(id))
        return false;

    auto&& et = entitiesPool[id];
    return et.visible;
}

void EntityCompStorage::setEntityPivotAt(const Math::Vec2& pivot, uint32_t id){

    if (Base::ID::isInvalidID(id))
        return;
    transformPivotMap[id] = pivot;
}
Math::Vec2 EntityCompStorage::getEntityPivotAt(uint32_t id) const{

    if (Base::ID::isInvalidID(id) || !transformPivotMap.contains(id))
        return {};
    return transformPivotMap.at(id);
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return {};

    auto&& et = entitiesPool[id];
    if (Base::ID::isInvalidID(et.transformId))
        return {};
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return;
    auto&& et                      = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}

Math::Mat33 EntityCompStorage::getEntityGlobalMat33At(const Base::ID::KeyUint64 id)
{
    if (id.isIDInvalid())
        return Math::Mat33::makeIdentity();

    if (id.isIIDValid() && entityInsGlobalMat33Map.contains(id))
        return entityInsGlobalMat33Map[id];

    if (entityGlobalMat33Map.contains(id.protoId()))
        return entityGlobalMat33Map[id.protoId()];

    return Math::Mat33::makeIdentity();
}
Math::Mat33 EntityCompStorage::getEntityGlobalMat33At(uint32_t id)
{
    if (Base::ID::isInvalidID(id))
        return Math::Mat33::makeIdentity();

    return entityGlobalMat33Map[id];
}

void EntityCompStorage::checkIds(std::vector<Base::ID::KeyUint64>& edis)
{
    if (edis.empty())
        return;

    std::vector<Base::ID::KeyUint64> ids{};
    auto                             tot = edis.size();
    for (auto i = 0; i < tot; ++i)
    {
        auto&& key = edis[i];
        if (key.protoId() == 0)
            continue;

        if (key.flags() > 0)
        {
            ids.push_back(key);
            continue;
        }

        auto protoId = key.protoId();
        if (entitiesPool.isInvalid(protoId))
        {
            ids.push_back(key);
            continue;
        }

        auto&& et = entitiesPool[protoId];
        if (!et.visible)
            continue;

        ids.push_back(key);
    }
    if (edis.size() != ids.size())
    {
        edis = ids;
    }
    if (edis.size() <= 1)
    {
        return;
    }
    std::sort(edis.begin(), edis.end(), [&](Base::ID::KeyUint64 a, Base::ID::KeyUint64 b) {
        return hierarchyIndexMap[a] < hierarchyIndexMap[b];
    });
}


void EntityCompStorage::updateHierarchyInfo()
{
    uint32_t index = 0;
    traverseSortIndex(0, index);
    //traverseSortIndexAndBuildGlobalMat(0, index, {});
    traverseBuildGlobalMat(0, {});
    propagateVisibility(0);
}

void EntityCompStorage::traverseSortWithShadowEffect(const Base::ID::KeyUint64& srcKey, uint32_t protoId, uint32_t& index)
{
    if (!entitiesPool.isValid(protoId))
        return;

    auto&& et = entitiesPool[protoId];
    if (Base::ID::isInvalidID(et.shadingId))
        return;
    auto&& shadingEt = shaderingEntitiesPool[et.shadingId];
    if (Base::ID::isInvalidID(shadingEt.shadingDescId))
        return;

    auto&& shdDesc = shaderingDescPool[shadingEt.shadingDescId];
    if (shdDesc.flags > 0 && shadingShadowIdMap.contains(shadingEt.shadingDescId))
    {
        auto&& effects = shadingShadowIdMap[shadingEt.shadingDescId];
        for (auto ef : effects)
        {
            auto&& key = Base::ID::KeyUint64::makeWithEffectShadow(srcKey, ef);
            printf("traverseSortWithShadowEffect(), key:%s index: %u\n", key.idToString().c_str(), index);
            hierarchyIndexMap[key] = index++;
        }
    }
}


void EntityCompStorage::traverseSortIndexWithInstance(uint32_t iid, uint32_t protoId, uint32_t& index)
{
    auto&& key = Base::ID::KeyUint64::make(protoId, iid);

    traverseSortWithShadowEffect(key, protoId, index);

    hierarchyIndexMap[key] = index++;

    auto&& et             = entitiesPool[protoId];
    auto   effectiveProto = Base::ID::isValidID(et.prototypeId) ? et.prototypeId : protoId;

    for (auto child = hierarchiesPool[effectiveProto].firstChild;
         Base::ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        auto& cet = entitiesPool[child];
        if (Base::ID::isValidID(cet.prototypeId))
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

    auto&& key = Base::ID::KeyUint64::make(protoId);
    traverseSortWithShadowEffect(key, protoId, index);
    hierarchyIndexMap[key] = index++;

    auto&& et = entitiesPool[protoId];
    if (Base::ID::isValidID(et.prototypeId))
    {
        auto iid = protoId;
        protoId  = et.prototypeId;

        for (auto child = hierarchiesPool[protoId].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            traverseSortIndexWithInstance(iid, child, index);
        }
        return;
    }

    for (auto child = hierarchiesPool[protoId].firstChild;
         Base::ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseSortIndex(child, index);
    }
}

void EntityCompStorage::traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat)
{
    auto&& et = entitiesPool[etId];

    //printf("traverseSortIndexAndBuildGlobalMat(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[Base::ID::KeyUint64::make(etId)] = index++;
    if (Base::ID::isValidID(et.transformId))
    {
        auto&& tr = transformsPool[et.transformId];
        //printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

        // get translation only
        auto&& parentTrans = parentMat.getXY();
        //printf("    ins parentTrans pos(x=%f,y=%f)\n", parentTrans.x, parentTrans.y);

        auto&& worldMat = Math::Mat33::makeTranslate(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);

        entityGlobalMat33Map[etId] = worldMat;
        //worldMat.print();
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         Base::ID::isValidID(child);
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
        if (Base::ID::isInvalidID(id)) return;
        topoOrder.push_back(id);
        topoIndex[id] = index++;
        for (uint32_t child = hierarchiesPool[id].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfs(child);
        }
    };
    for (auto r : roots) dfs(r);
}
void EntityCompStorage::traverseBuildGlobalMatInstance(uint32_t iid, const Math::Mat33& parentMat, Component::UnitInstanceMap& insMap)
{
    if (Base::ID::isInvalidID(iid)) return;

    auto&&      et       = entitiesPool[iid];
    Math::Mat33 worldMat = parentMat;

    if (Base::ID::isValidID(et.transformId))
    {
        auto&& tr = transformsPool[et.transformId];

        // compose: world = parentTranslation + local (translation-only from parent)
        // extract parent translation:
        auto&& parentTrans = parentMat.getXY();
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy);
    }

    auto&& key = Base::ID::KeyUint64::make(iid, 0);
    // store in insMap (prototype node id = iid if called on prototype tree)
    insMap.map[key] = {key};

    // traverse prototype's children (note: when instancing, we traverse prototype hierarchy)
    for (auto child = hierarchiesPool[iid].firstChild;
         Base::ID::isValidID(child);
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
        if (Base::ID::isValidID(protoEt.transformId))
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
        auto&& key                   = Base::ID::KeyUint64::make(protoId, etIID);
        map.map[key]                 = {key};
        entityInsGlobalMat33Map[key] = worldMat;

        for (auto child = hierarchiesPool[protoId].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfsEntity(etIID, child, worldMat);
        }
    };

    auto&& key                   = Base::ID::KeyUint64::make(protoId, iid);
    entityInsGlobalMat33Map[key] = instanceParentMat;

    for (auto child = hierarchiesPool[protoId].firstChild;
         Base::ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        dfsEntity(iid, child, instanceParentMat);
    }
    // store into instanceStorage
    instanceStorage[iid] = std::move(map);
}
void EntityCompStorage::traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat)
{
    if (Base::ID::isInvalidID(etId) || entitiesPool.isInvalid(etId)) return;

    auto&&      et       = entitiesPool[etId];
    Math::Mat33 worldMat = parentMat;

    if (Base::ID::isValidID(et.transformId))
    {
        auto&&      tr = transformsPool[et.transformId];
        Math::Mat33 objMat;
        objMat.setTo(tr.x, tr.y, 1, 1, tr.rotation);

        worldMat.append(objMat);

        /*
        auto&& parentTrans = parentMat.getXY();
        auto&& tr          = transformsPool[et.transformId];
        if (tr.rotation != 0)
        {
            Math::Vec2 pos = {parentTrans.x + tr.x, parentTrans.y + tr.y};
            Math::Vec2 sv  = {tr.sx, tr.sy};
            worldMat.setTo(pos.x, pos.y, sv.x, sv.y, tr.rotation);
            // rotating around the entity bounds center
            Math::Vec2 centerPivot{0.5f, 0.5f};
            auto       srcCV = worldMat.mapPoint(centerPivot);
            Math::Vec2 dstCV = pos + sv * 0.5f;
            pos += dstCV - srcCV;
            worldMat.setXY(pos);
        }
        else
        {
            worldMat.identity();
            worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
            worldMat.setScaleXY(tr.sx, tr.sy);
        }
        //*/
    }

    entityGlobalMat33Map[etId] = worldMat;

    // if entity is instance of prototype, build instance map using prototype tree
    if (Base::ID::isValidID(et.prototypeId))
    {
        //printf("EntityCompStorage::traverseBuildGlobalMat() has prototypeId: %u\n", et.prototypeId);
        if (Base::ID::isValidID(hierarchiesPool[etId].firstChild))
        {
            printf("[Warning] entity %u is an instance, but has children in hierarchy ignored.\n", etId);
        }
        traverseBuildGlobalMatPrototypeUnderInstance(etId, et.prototypeId, worldMat);
        return;
    }

    // recurse children (entity children, not prototype children)
    for (auto child = hierarchiesPool[etId].firstChild;
         Base::ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        traverseBuildGlobalMat(child, worldMat);
    }
}

void EntityCompStorage::traverseBuildGlobalMat(uint32_t etId)
{
    if (Base::ID::isInvalidID(etId) || entitiesPool.isInvalid(etId)) return;
    auto&& parentMat = getEntityParentGlobalMatAt(etId);
    traverseBuildGlobalMat(etId, parentMat);
}

void EntityCompStorage::markSubtreeDirty(uint32_t rootId)
{
    if (Base::ID::isInvalidID(rootId)) return;

    std::vector<uint32_t> stack{rootId};
    while (!stack.empty())
    {
        auto id = stack.back();
        stack.pop_back();
        entitiesPool[id].dirty = true;
        entityGlobalMat33Map.erase(id); // optional: clear old cached matrix
        for (auto c = hierarchiesPool[id].firstChild; Base::ID::isValidID(c); c = hierarchiesPool[c].next)
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
                for (auto c = hierarchiesPool[id].firstChild; Base::ID::isValidID(c); c = hierarchiesPool[c].next)
                    stack.push_back(c);
            }
        }
    }
}

void EntityCompStorage::collectShadowEffect(const Base::ID::KeyUint64& srcKey, uint32_t protoId, std::vector<Base::ID::KeyUint64>& ids)
{
    if (entitiesPool.isInvalid(protoId))
        return;

    auto&& et = entitiesPool[protoId];
    if (Base::ID::isInvalidID(et.shadingId))
        return;

    auto&& shadingEt = shaderingEntitiesPool[et.shadingId];
    if (Base::ID::isInvalidID(shadingEt.shadingDescId))
        return;
    auto&& shdDesc = shaderingDescPool[shadingEt.shadingDescId];
    if (shdDesc.flags == 0 || !shadingShadowIdMap.contains(shadingEt.shadingDescId))
        return;

    auto&& effects = shadingShadowIdMap[shadingEt.shadingDescId];
    for (auto ef : effects)
    {
        auto&& key = Base::ID::KeyUint64::makeWithEffectShadow(srcKey, ef);
        ids.emplace_back(key);
    }
}

void EntityCompStorage::collectAllEntitiesWithInstance(const Base::ID::KeyUint64& etId, std::vector<Base::ID::KeyUint64>& ids)
{
    auto                             iid = etId.iid();
    std::vector<Base::ID::KeyUint64> stack;
    stack.reserve(256);
    stack.emplace_back(etId);

    while (!stack.empty())
    {
        auto id = stack.back();
        stack.pop_back();

        ids.emplace_back(id);

        auto protoId = id.protoId();
        if (entitiesPool.isInvalid(protoId))
            continue;

        auto&&   et     = entitiesPool[protoId];
        uint32_t currId = Base::ID::isValidID(et.prototypeId) ? et.prototypeId : protoId;

        for (auto child = hierarchiesPool[currId].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            stack.push_back(Base::ID::KeyUint64::make(child, iid));
        }
    }
}


void EntityCompStorage::collectAllEntities(const Base::ID::KeyUint64& etId, std::vector<Base::ID::KeyUint64>& ids)
{
    if (Base::ID::isInvalidID(etId))
        return;


    collectShadowEffect(etId, etId.protoId(), ids);
    ids.emplace_back(etId);

    auto protoId = etId.protoId();
    auto iid     = etId.iid();

    for (auto&& insItem : instanceStorage)
    {
        auto&  ins = insItem.second;
        auto&& key = Base::ID::KeyUint64::make(protoId, ins.iid);
        if (ins.map.contains(key))
        {
            collectAllEntitiesWithInstance(key, ids);
        }
    }

    if (iid > 0)
    {
        //printf("collectAllEntities() has a new instance entity.\n");
        auto&&   protoEntity    = entitiesPool[protoId];
        uint32_t effectiveProto = Base::ID::isValidID(protoEntity.prototypeId) ? protoEntity.prototypeId : protoId;

        for (auto child = hierarchiesPool[effectiveProto].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            auto&    cet        = entitiesPool[child];
            uint32_t childProto = Base::ID::isValidID(cet.prototypeId) ? cet.prototypeId : child;
            collectAllEntitiesWithInstance(Base::ID::KeyUint64::make(childProto, iid), ids);
        }
        return;
    }
    auto& et = entitiesPool[protoId];
    if (Base::ID::isValidID(et.prototypeId))
    {
        for (auto child = hierarchiesPool[et.prototypeId].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            auto&    cet        = entitiesPool[child];
            uint32_t childProto = Base::ID::isValidID(cet.prototypeId) ? cet.prototypeId : child;
            collectAllEntitiesWithInstance(Base::ID::KeyUint64::make(child, protoId), ids);
        }
        return;
    }
    for (auto child = hierarchiesPool[protoId].firstChild;
         Base::ID::isValidID(child);
         child = hierarchiesPool[child].next)
    {
        collectAllEntities(Base::ID::KeyUint64::make(child), ids);
    }
}

void EntityCompStorage::updateAllInstanceGlobalMats(const Base::ID::KeyUint64& etId)
{
    auto protoId = etId.protoId();
    if (Base::ID::isInvalidID(protoId))
        return;
    if (entitiesPool.isInvalid(protoId))
    {
        return;
    }
    std::function<void(uint32_t iid, uint32_t protoId, const Math::Mat33&)> dfsEntity = [&](uint32_t iid, uint32_t protoId, const Math::Mat33& parentMat) {
        Math::Mat33 worldMat = parentMat;
        auto&       protoEnt = entitiesPool[protoId];
        if (Base::ID::isValidID(protoEnt.transformId))
        {
            auto&  tr          = transformsPool[protoEnt.transformId];
            auto&& parentTrans = parentMat.getXY();
            worldMat.identity();
            worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
            worldMat.setScaleXY(tr.sx, tr.sy);
        }
        auto&& key = Base::ID::KeyUint64::make(protoId, iid);

        entityInsGlobalMat33Map[key] = worldMat;

        for (auto child = hierarchiesPool[protoId].firstChild;
             Base::ID::isValidID(child);
             child = hierarchiesPool[child].next)
        {
            dfsEntity(iid, child, worldMat);
        }
    };

    for (auto&& insItem : instanceStorage)
    {
        auto&  ins = insItem.second;
        auto&& key = Base::ID::KeyUint64::make(protoId, ins.iid);
        if (ins.map.contains(key))
        {
            auto parentProtoId = getEntityParentIdAt(protoId);
            if (Base::ID::isValidID(parentProtoId))
            {
                auto&& parentKey = Base::ID::KeyUint64::make(parentProtoId, ins.iid);
                if (entityInsGlobalMat33Map.contains(parentKey))
                {
                    auto&& parentWMat = entityInsGlobalMat33Map[parentKey];
                    dfsEntity(ins.iid, protoId, parentWMat);
                }
            }
        }
    }
}

void EntityCompStorage::setPrototypeEntitiesDirty(uint32_t etId, bool dirty, uint16_t reserveSize)
{
    if (Base::ID::isInvalidID(etId) || entitiesPool.isInvalid(etId)) return;

    if (reserveSize < 32)
        reserveSize = 32;

    std::vector<uint32_t> stack;
    stack.reserve(reserveSize);
    stack.emplace_back(etId);

    while (!stack.empty())
    {
        auto id = stack.back();
        stack.pop_back();
        entitiesPool[id].dirty = dirty;
        for (auto c = hierarchiesPool[id].firstChild; Base::ID::isValidID(c); c = hierarchiesPool[c].next) stack.push_back(c);
    }
}

void EntityCompStorage::foreachBoundsWithEntityId(uint32_t eId, EntityBoundsResponseCallType callback)
{
    if (Base::ID::isInvalidID(eId))
        return;

    auto addShadowEffectBVHData = [&, this](const Base::ID::KeyUint64& key, const Math::Bounds& srcBounds) {
        auto protoId = key.protoId();
        if (entitiesPool.isInvalid(protoId)) { return; }

        auto&& et = entitiesPool[protoId];
        if (Base::ID::isInvalidID(et.shadingId)) { return; }

        auto&& shadingEt = shaderingEntitiesPool[et.shadingId];
        if (entitiesPool.isInvalid(shadingEt.shadingDescId)) { return; }

        auto&& desc = shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = shadingShadowIdMap[shadingEt.shadingDescId];

        auto&& wmat = getEntityGlobalMat33At(key);

        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            srcBounds.mat33MapTo(wm, vb);

            auto&& efKey = Base::ID::KeyUint64::makeWithEffectShadow(key, ef);
            callback(key, vb);
        }
    };

    Math::Bounds vb;

    std::vector<Base::ID::KeyUint64> ids{};
    collectAllEntities(Base::ID::KeyUint64::make(eId), ids);
    for (auto pid : ids)
    {
        if (pid.flags() > 0)
            continue;

        auto&& srcBounds = getEntityLocalBoundsAt(pid);
        auto   wm        = getEntityGlobalMat33At(pid);
        addShadowEffectBVHData(pid, srcBounds);

        srcBounds.mat33MapTo(wm, vb);
        callback(pid, vb);
    }
}
void EntityCompStorage::propagateVisibility(uint32_t rootId, bool visible)
{
    std::function<void(uint32_t, bool)> dfs =
        [&, this](uint32_t id, bool parentVisible) {
            auto&& e        = entitiesPool[id];
            e.globalVisible = e.visible && parentVisible;

            uint32_t child = hierarchiesPool[id].firstChild;
            while (Base::ID::isValidID(child))
            {
                dfs(child, e.globalVisible);
                child = hierarchiesPool[child].next;
            }
        };

    dfs(rootId, visible);
}

} // namespace Voxol::Scene