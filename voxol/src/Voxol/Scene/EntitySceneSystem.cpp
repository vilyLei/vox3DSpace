#include "EntitySceneSystem.h"
#include <algorithm>

namespace Voxol::Scene
{

EntitySceneSystem::SP EntitySceneSystem::make()
{
    auto sp = std::make_shared<EntitySceneSystem>();
    return sp;
}

void EntitySceneSystem::initalize(const std::string& configFileName)
{
    if (entityStorage)
        return;

    entityStorage = EntityUnitStorage::make();
    if (configFileName.empty())
    {
        entityStorage->initalize(512);
    }
    else
    {
        entityStorage->initalizeFromFile(configFileName);
    }

    auto& storage               = entityStorage->comp;
    auto& entitiesPool          = storage->entitiesPool;
    auto& shaderingEntitiesPool = storage->shaderingEntitiesPool;
    auto& shaderingDescPool     = storage->shaderingDescPool;
    auto& transformsPool        = storage->transformsPool;
    auto& instanceStorage       = storage->instanceStorage;


    auto addShadowEffectBVHData = [&](const Base::ID::KeyUint64& key, const Math::Mat33& wmat) {
        auto protoId = key.protoId();

        if (entitiesPool.isInvalid(protoId)) { return; }

        auto&& et = entitiesPool[protoId];

        if (Base::ID::isInvalidID(et.shadingId)) { return; }

        auto& shadingEt = shaderingEntitiesPool[et.shadingId];
        if (Base::ID::isInvalidID(shadingEt.shadingDescId)) { return; }

        auto& desc = shaderingDescPool[shadingEt.shadingDescId];
        if (desc.flags == 0) { return; }
        auto&& efs = storage->shadingShadowIdMap[shadingEt.shadingDescId];

        auto&&       srcBounds = storage->getEntityLocalBoundsAt(protoId);
        Math::Bounds vb;
        for (auto& ef : efs)
        {
            auto&& shdData = storage->effectShadowMap[ef];
            auto   wm      = wmat;
            wm.offsetXY(shdData.offset);
            //Component::defaultRect.mat33MapTo(wm, vb);
            srcBounds.mat33MapTo(wm, vb);
            auto&& shadowKey = Base::ID::KeyUint64::makeWithEffectShadow(key, ef);
            bvh->addItem(shadowKey, vb);
            storage->effectShadowEntityMap[shadowKey] = {shadowKey, key, ef};
        }
    };

    auto updateProtoEtBVHData = [&](auto& et) {
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        bvh->addItem(Base::ID::KeyUint64::make(et.id), vb);
        auto& wmats = storage->entityInsGlobalMat33Map;
        if (!instanceStorage.contains(et.id))
            return;

        auto&& insMap = instanceStorage[et.id];
        for (auto& item : insMap.map)
        {
            auto&& mat = wmats[item.first];
            addShadowEffectBVHData(item.first, mat);
            auto&& srcBounds = storage->getEntityLocalBoundsAt(item.first);

            srcBounds.mat33MapTo(mat, vb);
            bvh->addItem(item.first, vb);
        }
    };

    entitiesPool.forEach([&](auto& et, uint32_t index) {
        if (Base::ID::isInvalidID(et.transformId))
            return;


        auto&& srcBounds = storage->getEntityLocalBoundsAt(et.id);
        if (srcBounds.isEmpty())
        {
            printf("et.id: %d, bounds is empty().\n", et.id);
            return;
        }

        if (Base::ID::isValidID(et.prototypeId))
        {
            // instance entity
            updateProtoEtBVHData(et);
            return;
        }

        auto&& key = Base::ID::KeyUint64::make(et.id);
        addShadowEffectBVHData(key, storage->getEntityGlobalMat33At(et.id));
        auto&& vb = storage->getEntityGlobalBoundsAt(et.id);
        //printf("et.id: %d, add bvh vb: \n", et.id);
        //vb.print();
        bvh->addItem(key, vb);
    });

    bvh->build();


    if (!interSrcSys)
    {

        auto compStorage = entityStorage->comp;


        actionSys              = Intent::ActionSystem::make();
        actionSys->compStorage = compStorage;

        interSrcSys              = Intent::InteractionSourceSystem::make();
        interSrcSys->actionSys   = actionSys;
        interSrcSys->compStorage = compStorage;
        interSrcSys->initialize();

        auto& fileParser = entityStorage->descParser.fileParser;
        //interSrcSys->actionIDMap = fileParser.interactionIDMap;

        auto& interactionSrcMap = fileParser.interactionSrcMap;
        for (auto& item : interactionSrcMap)
        {
            auto& srcNode = item.second;
            interSrcSys->addSource(srcNode);
        }
    }

    createEntities(1);
}

void EntitySceneSystem::createEntities(int total)
{
    if (total < 1)
        return;

    auto       compStorage = entityStorage->comp;
    Math::Vec2 pos{200, 100};
    uint32_t   srcEtId = 1;
    bool       biulding = false;
    std::vector<uint32_t> ids;
    for (auto i = 0; i < total; i++)
    {
        
        auto id = compStorage->appendEntityCopyFromId(srcEtId);
        if (Base::ID::isInvalidID(id))
            continue;

        ids.push_back(id);
    }
    if (!ids.empty())
    {
        compStorage->updateHierarchyInfo();
        for (auto id : ids)
        {
            auto&& key = Base::ID::KeyUint64::make(id);
            auto&& vb = compStorage->getEntityGlobalBoundsAt(id);
            compStorage->setEntityGlobalXYAt(pos, id);
            pos += {10, 10};
            bvh->addItem(key, vb);
        }
        bvh->build();
    }
}
void EntitySceneSystem::update()
{
    interSrcSys->update();
}
int EntitySceneSystem::drawQuery(const Math::VxRect& wbounds, int phase)
{
    queriedEIds.clear();
    bvh->queryBounds(wbounds, queriedEIds);

    entityStorage->comp->checkIds(queriedEIds);

    return static_cast<int>(queriedEIds.size());
}

const std::vector<Base::ID::KeyUint64> EntitySceneSystem::getQueriedEIds() const
{
    return queriedEIds;
}
void EntitySceneSystem::clear()
{
}

void EntitySceneSystem::updateBoundsWithEntityId(uint32_t eId, EntityBoundsResponseCallType callback)
{
    entityStorage->comp->foreachBoundsWithEntityId(eId, callback);
}
} // namespace Voxol::Scene