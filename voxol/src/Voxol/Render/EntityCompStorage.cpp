#include "EntityCompStorage.h"
#include <algorithm>

namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}

bool EntityCompStorage::hasParentAt(uint32_t id) const
{
    if (id == Component::INVALID_ID || id >= hierarchiesPool.capacity())
        return false;
    auto&& hier = hierarchiesPool[id];
    return hier.parent != Component::INVALID_ID;
}

bool EntityCompStorage::hasChildAt(uint32_t id) const
{
    if (id == Component::INVALID_ID || id >= hierarchiesPool.capacity())
        return false;
    auto& hier = hierarchiesPool[id];
    return hier.firstChild != Component::INVALID_ID;
}

Component::UnitTransform EntityCompStorage::getEntityLocalTransAt(uint32_t id)
{
    if (id == Component::INVALID_ID)
        return Component::DefaultTrans;

    auto&& et = entitiesPool[id];
    if (et.transformId == Component::INVALID_ID)
        return Component::DefaultTrans;

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
    if (id == Component::INVALID_ID)
        return {};
    auto parentId = getEntityParentIdAt(id);
    return getEntityGlobalMatAt(parentId);
}
Math::Mat33 EntityCompStorage::getEntityGlobalMatAt(uint32_t id)
{
    if (id == Component::INVALID_ID || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        Math::Mat33 mat;
        mat.identity();
        return mat;
    }
    return entityGlobalMat33Map[id];
}

Math::Bounds EntityCompStorage::getEntityGlobalBoundsAt(uint32_t id)
{
    if (id == Component::INVALID_ID || id >= hierarchiesPool.capacity() || !entityGlobalMat33Map.contains(id))
    {
        return {};
    }

    Math::Bounds bounds = {0, 0, 1, 1};
    Math::Bounds tb;
    bounds.mat33MapTo(entityGlobalMat33Map[id], tb);
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
    if (id == Component::INVALID_ID)
        return {};
    return entityGlobalMat33Map[id].getXY();
}
void EntityCompStorage::setEntityGlobalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (id == Component::INVALID_ID)
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
    auto&& lpv                       = matInv.mapPoint(wpv);
    transformsPool[et.transformId].pos() = lpv;
}

Math::Mat33 EntityCompStorage::getEntityWorldMatWithoutScale(uint32_t id)
{
    auto&& trans = getEntityLocalTransAt(id);
    printf("getEntityWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
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
    printf("getEntityParentWorldMatWithoutScale() id: %u, trans(x=%f,y=%f,sx=%f,sy=%f)\n", id, trans.x, trans.y, trans.sx, trans.sy);
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
    if (id == Component::INVALID_ID)
        return {};

    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}

void EntityCompStorage::setEntityLocalXYAt(const Math::Vec2& pv, uint32_t id)
{
    if (id == Component::INVALID_ID)
        return;
    auto&& et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    trans.pos() = pv;
}

void EntityCompStorage::traverseBuildGlobalMat(uint32_t etId, const Math::Mat33& parentMat)
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

        printf("traverseBuildGlobalMat(), etId:%u, tr(x=%f,y=%f), ptr(x=%f,y=%f)\n", etId, tr.x, tr.y, parentTrans.x, parentTrans.y);

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
        traverseBuildGlobalMat(child, entityGlobalMat33Map[etId]);
    }
}
void EntityCompStorage::getIdsFromId(uint32_t etId, std::vector<uint32_t>& ids)
{
    if (etId == Component::INVALID_ID)
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
    if (id == Component::INVALID_ID)
        return {};

    auto&& et = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{
    if (id == Component::INVALID_ID)
        return;
    auto&& et                       = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}
Math::Mat33 EntityCompStorage::getEntityGlobalMat33At(uint32_t id)
{
    if (id == Component::INVALID_ID)
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

void EntityCompStorage::traverseSortIndexAndBuildGlobalMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat)
{
    constexpr auto InvalidID = Component::INVALID_ID;
    auto&&         et        = entitiesPool[etId];

    printf("traverseSortIndexAndBuildGlobalMat(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[etId] = index++;
    if (et.transformId != InvalidID)
    {
        auto& tr = transformsPool[et.transformId];
        printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

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
} // namespace Voxol::Render