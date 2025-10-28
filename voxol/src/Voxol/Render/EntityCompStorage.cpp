#include "EntityCompStorage.h"
#include <algorithm>

namespace Voxol::Render
{
EntityCompStorage::SP EntityCompStorage::make()
{
    auto sp = std::make_shared<EntityCompStorage>();
    return sp;
}

Component::UnitTransform EntityCompStorage::getEntityTransformAt(uint32_t id)
{
    if (id == Component::INVALID_ID)
        return {};

    auto& et = entitiesPool[id];
    return transformsPool[et.transformId];
}
void EntityCompStorage::setEntityTransformAt(const Component::UnitTransform& trans, uint32_t id)
{
    if (id == Component::INVALID_ID)
        return;
    auto& et                       = entitiesPool[id];
    transformsPool[et.transformId] = trans;
}
Math::Mat33 EntityCompStorage::getEntityMat33At(uint32_t id)
{
    if (id == Component::INVALID_ID)
        return {};

    return entityWorldMat33Map[id];
}
Math::Vec2 EntityCompStorage::getEntityXYAt(uint32_t id)
{
    if (id == Component::INVALID_ID)
        return {};

    auto&  et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    return {trans.x, trans.y};
}
void EntityCompStorage::setEntityXYAt(const Math::Vec2& pos, uint32_t id)
{
    if (id == Component::INVALID_ID)
        return;
    auto&  et    = entitiesPool[id];
    auto&& trans = transformsPool[et.transformId];
    trans.x      = pos.x;
    trans.y      = pos.y;
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

void EntityCompStorage::checkWMaps(std::vector<uint32_t>& edis)
{
}

void EntityCompStorage::updateHierarchyInfo()
{
    uint32_t index = 0;
    //traverseSortIndex(0, index);
    traverseSortIndexAndBuildWorldMat(0, index, {});
}
void EntityCompStorage::traverseSortIndexAndBuildWorldMat(uint32_t etId, uint32_t& index, const Math::Mat33& parentMat)
{
    constexpr auto InvalidID = Component::INVALID_ID;
    auto&&         et        = entitiesPool[etId];

    printf("traverseSortIndexAndBuildWorldMat(), etId: %d, index: %d\n", etId, index);
    hierarchyIndexMap[etId] = index++;
    if (et.transformId != InvalidID)
    {
        auto& tr = transformsPool[et.transformId];
        printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

        // 当前节点的本地矩阵
        Math::Mat33 localMat;
        localMat.identity();
        localMat.setScaleXY(tr.sx, tr.sy);
        //localMat.setRotation(tr.rotation);
        localMat.setXY(tr.x, tr.y);

        // 仅传递平移：提取父矩阵的 translation
        auto&& parentTrans = parentMat.getXY();

        // 构造新的 world matrix：仅叠加平移
        Math::Mat33 worldMat;
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy); // 自身 scale 不受父级影响

        entityWorldMat33Map[etId] = worldMat;
        worldMat.print();
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         child != InvalidID;
         child = hierarchiesPool[child].next)
    {
        traverseSortIndexAndBuildWorldMat(child, index, entityWorldMat33Map[etId]);
    }
}

void EntityCompStorage::traverseBuildWorldMat(uint32_t etId, const Math::Mat33& parentMat)
{
    constexpr auto InvalidID = Component::INVALID_ID;
    auto&&         et        = entitiesPool[etId];

    printf("traverseSortIndex(), etId: %d\n", etId);
    if (et.transformId != InvalidID)
    {
        auto& tr = transformsPool[et.transformId];
        printf("    tr(x=%f,y=%f,sx=%f,sy=%f)\n", tr.x, tr.y, tr.sx, tr.sy);

        // 当前节点的本地矩阵
        Math::Mat33 localMat;
        localMat.identity();
        localMat.setScaleXY(tr.sx, tr.sy);
        //localMat.setRotation(tr.rotation);
        localMat.setXY(tr.x, tr.y);

        // 仅传递平移：提取父矩阵的 translation
        auto&& parentTrans = parentMat.getXY();

        // 构造新的 world matrix：仅叠加平移
        Math::Mat33 worldMat;
        worldMat.identity();
        worldMat.setXY(parentTrans.x + tr.x, parentTrans.y + tr.y);
        worldMat.setScaleXY(tr.sx, tr.sy); // 自身 scale 不受父级影响

        entityWorldMat33Map[etId] = worldMat;
        worldMat.print();
    }

    for (auto child = hierarchiesPool[etId].firstChild;
         child != InvalidID;
         child = hierarchiesPool[child].next)
    {
        traverseBuildWorldMat(child, entityWorldMat33Map[etId]);
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