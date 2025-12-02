#include "InteractionSourceSystem.h"
namespace Voxol::Intent
{

InteractionSourceSystem::SP InteractionSourceSystem::make()
{
    auto sp = std::make_shared<InteractionSourceSystem>();
    return sp;
}
void InteractionSourceSystem::initialize()
{
    /*
    using namespace Interaction;

    InteractionSource srcNode;
    srcNode.id = Base::ID::KeyUint64::make(4);

    // 模拟DSL输入后得到的初始化,
    // ID::KeyUint64::make(1) 表示的是场景层次节点1模拟按钮, 即 entity(1)
    // ID::KeyUint64::make(5) 表示的是场景层次节点5模拟一个面板, 即 entity(5)

    auto btnID   = Base::ID::KeyUint64::make(3);
    auto panelID = Base::ID::KeyUint64::make(5);

    bool                 visible = true;
    InteractionTargetSet outTar;
    outTar.flag = static_cast<uint8_t>(MouseStatus::Out);
    outTar.targets.push_back({btnID, "default", 0xff660000, visible});
    srcNode.tars[outTar.flag] = outTar;

    InteractionTargetSet overTar;
    overTar.flag = static_cast<uint8_t>(MouseStatus::Over);
    overTar.targets.push_back({btnID, "default", 0xff663300, visible});
    srcNode.tars[overTar.flag] = overTar;

    InteractionTargetSet moveTar;
    moveTar.flag = static_cast<uint8_t>(MouseStatus::Move);
    moveTar.targets.push_back({btnID, "default", 0xff663300, visible});
    srcNode.tars[moveTar.flag] = moveTar;

    InteractionTargetSet downTar;
    downTar.flag = static_cast<uint8_t>(MouseStatus::Down);
    downTar.targets.push_back({btnID, "default", 0xff883300, visible});
    srcNode.tars[downTar.flag] = downTar;

    // mouse up的时候会改变按钮entit(1)的颜色，同时也会关闭 entity(5) 这个面板
    InteractionTargetSet upTar;
    upTar.flag = static_cast<uint8_t>(MouseStatus::Up);
    // only chagne the btn color
    upTar.targets.push_back({btnID, "default", 0xff663300, visible});
    // close the entity(5) displaying when the mouse up action happened.
    visible = false;
    upTar.targets.push_back({panelID, "default", 0xff008888, visible});

    srcNode.tars[upTar.flag] = upTar;

    addSource(srcNode);
    //*/
}


void InteractionSourceSystem::addSource(const Interaction::InteractionSource& srcNode)
{
    srcMap[srcNode.id] = srcNode;
}

void InteractionSourceSystem::foreachSrcNode(const Interaction::SourceCallbackType& callback)
{
    if (!callback)
        return;

    for (auto& item : srcMap)
    {
        auto& obj = item.second;
        callback(obj);
        obj.flags = Interaction::MouseStatus::None;
        obj.dirty = false;
    }
}

void InteractionSourceSystem::singalParse(Interaction::InteractionSource& srcNode, bool selectionFlag, const std::string& actDesc)
{
    if (selectionFlag)
    {
        if (srcNode.hit && actDesc == "moving")
        {
            //srcNode.dirty = true;
            //srcNode.actDesc = "moving";
            //printf("InteractionSourceSystem::singalParse() hit srcNode.actDesc: %s\n", srcNode.actDesc.c_str());
            srcNode.flags.set(Interaction::MouseStatus::Move);
        }
        if (!srcNode.hit && actDesc != "none")
        {
            srcNode.flags.set(Interaction::MouseStatus::Over);

            srcNode.hit     = true;
            srcNode.dirty   = true;
            srcNode.actDesc = "over";
            printf("InteractionSourceSystem::singalParse() hit obj.actDesc: %s\n", srcNode.actDesc.c_str());
        }
        if (srcNode.hit && !srcNode.active && actDesc == "down")
        {
            srcNode.flags.set(Interaction::MouseStatus::Down);
            srcNode.dirty   = true;
            srcNode.active  = true;
            srcNode.actDesc = actDesc;
            printf("InteractionSourceSystem::singalParse() hit srcNode.act: %s\n", srcNode.actDesc.c_str());
        }
    }
    else
    {
        if (srcNode.hit && actDesc != "")
        {
            srcNode.flags.set(Interaction::MouseStatus::Out);
            srcNode.dirty   = true;
            srcNode.hit     = false;
            srcNode.actDesc = "out";
            printf("InteractionSourceSystem::singalParse() hit srcNode.actDesc: %s\n", srcNode.actDesc.c_str());
        }
    }
    if (srcNode.active && actDesc == "up")
    {
        srcNode.flags.set(Interaction::MouseStatus::Up);
        srcNode.dirty   = true;
        srcNode.active  = false;
        srcNode.actDesc = actDesc;
        printf("InteractionSourceSystem::singalParse() hit srcNode.act: %s\n", srcNode.actDesc.c_str());
    }
}

bool InteractionSourceSystem::containsSrcId(const Base::ID::KeyUint64& srcId)
{
    if (!compStorage)
        return true;

    return compStorage->interactionIDMap.contains(srcId.protoId());
}

Base::ID::KeyUint64 InteractionSourceSystem::findSrcId(const std::vector<Base::ID::KeyUint64>& qeIds)
{
    if (!compStorage)
        return Base::ID::INVALID_KEY;

    auto tot = qeIds.size();
    auto& interIdMap = compStorage->interactionIDMap;

    for (auto i = 0; i < tot; i++)
    {
        //if (actionIDMap.contains(qeIds[i].protoId()))
        //    return qeIds[i];

        if (interIdMap.contains(qeIds[i].protoId()))
            return qeIds[i];
    }
    return Base::ID::INVALID_KEY;
}

/// 由其他交互事件响应逻辑产生触发信号
void InteractionSourceSystem::updateSourceAct(const Base::ID::KeyUint64& srcId, const std::string& actDesc, const Scene::Component::UnitLocation& location)
{
    for (auto& item : srcMap)
    {
        singalParse(item.second, srcId == item.first, actDesc);
    }
}

void InteractionSourceSystem::execActToDsiplay(Interaction::InteractionSource& srcNode, Interaction::MouseStatus status, std::string actDesc)
{

    auto modifyDstFunc = [this](const Interaction::InteractionSource& src, Interaction::InteractionTargetDesc& desc, std::string actDesc) {

        if (!compStorage)
            return;

        //desc.apply();

        printf("InteractionSourceSystem::execActToDsiplay(), desc.type: %s, desc.command: %s\n", desc.type.c_str(), desc.command.c_str());

        auto etId = desc.entityKey.protoId();

        auto&& entity            = compStorage->entitiesPool[etId];
        auto&  shaderingEntities = compStorage->shaderingEntitiesPool;
        auto&  shaderingDescVec  = compStorage->shaderingDescPool;

        if (Base::ID::isInvalidID(entity.shadingId))
            return;

        auto& shadingEt = shaderingEntities[entity.shadingId];
        if (Base::ID::isInvalidID(shadingEt.shadingDescId))
        {
            return;
        }
        auto& shdDesc  = shaderingDescVec[shadingEt.shadingDescId];
        if (desc.type == "change-color")
        {
            shdDesc.color = desc.color;
        }
        else if (desc.type == "change-visible")
        {
            entity.visible = desc.visible;
        }
        else if (desc.type == "toggle-visible")
        {
            entity.visible = !entity.visible;
        }
        printf("modifyDstFunc() etId: %d, actDesc: %s, color: 0x%x\n", etId, actDesc.c_str(), shdDesc.color);

        //compStorage->foreachBoundsWithEntityId(etId, [this](const Base::ID::KeyUint64& etId, const Math::Bounds& bounds) {
        //    tileSys->addDirtyBounds(bounds, 0);
        //});

        entityDirtyCall(0, desc.entityKey);
    };

    auto   key = static_cast<uint8_t>(status);
    auto&& tar = srcNode.tars[key];
    printf("InteractionSourceSystem::execActToDsiplay(), tar.targets.size(): %zu\n", tar.targets.size());
    for (auto& dst : tar.targets)
    {
        modifyDstFunc(srcNode, dst, actDesc);
    }
}

void InteractionSourceSystem::singalToBehavior(Interaction::InteractionSource& srcNode)
{
    if (srcNode.flags.test(Interaction::MouseStatus::Over) && !srcNode.active)
    {
        execActToDsiplay(srcNode, Interaction::MouseStatus::Over, "over");
    }
    if (srcNode.flags.test(Interaction::MouseStatus::Out) && !srcNode.active)
    {
        execActToDsiplay(srcNode, Interaction::MouseStatus::Out, "out");
    }

    if (srcNode.flags.test(Interaction::MouseStatus::Down))
    {
        execActToDsiplay(srcNode, Interaction::MouseStatus::Down, "down");
    }
    if (srcNode.flags.test(Interaction::MouseStatus::Up))
    {
        if (srcNode.hit)
        {
            execActToDsiplay(srcNode, Interaction::MouseStatus::Up, "up");
        }
        else
        {
            execActToDsiplay(srcNode, Interaction::MouseStatus::Out, "up-out");
        }
    }
}

void InteractionSourceSystem::update()
{

    foreachSrcNode([&, this](Interaction::InteractionSource& srcNode) {
        if (!srcNode.dirty)
            return;
        if (srcNode.flags == Interaction::MouseStatus::None)
            return;
        singalToBehavior(srcNode);
    });

    auto& actionIdMap = compStorage->actionIDMap;
    for (auto&& item : actionIdMap) {

        auto etId = item.first;
        if (Base::ID::isInvalidID(etId))
            continue;

        auto&& entity = compStorage->entitiesPool[etId];
        if (Base::ID::isInvalidID(entity.transformId))
            continue;

        return;
        auto&& trans  = compStorage->transformsPool[entity.transformId];

        Math::Vec2 offset      = {1,0};
        auto&& originEtPos = compStorage->getEntityGlobalXYAt(etId);
        auto&& key   = Base::ID::KeyUint64::make(etId);
        //entityDirtyCall(0, key);
        //trans.rotation += 0.2f;
        //trans.pos() += offset;
        //compStorage->setEntityGlobalXYAt(originEtPos + offset, key.protoId());
        // 
        
        //auto&& parentMat = compStorage->getEntityParentGlobalMatAt(key.protoId());
        //compStorage->traverseBuildGlobalMat(key.protoId(), parentMat);
        //compStorage->updateAllInstanceGlobalMats(key);
        //entityDirtyCall(1, key);

    }
}


} // namespace Voxol::Intent