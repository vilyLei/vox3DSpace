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
// 由其他交互事件响应逻辑产生触发信号
void InteractionSourceSystem::updateSrcAct(const Render::ID::KeyUint64& srcId, const std::string& actDesc)
{
    for (auto& item : srcMap)
    {
        singalParse(item.second, srcId == item.first, actDesc);
    }
}

void InteractionSourceSystem::execActToDsiplay(Interaction::InteractionSource& srcNode, Interaction::MouseStatus status, std::string actDesc)
{

    auto modifyDstFunc = [this](const Interaction::InteractionTargetDesc& desc, std::string actDesc) {
        auto etId = desc.id.protoId();

        auto&& entity            = compStorage->entitiesPool[etId];
        auto&  shaderingEntities = compStorage->shaderingEntitiesPool;
        auto&  shaderingDescVec  = compStorage->shaderingDescPool;

        auto& shadingEt = shaderingEntities[entity.shadingId];
        if (Render::ID::isInvalidID(shadingEt.shadingDescId))
        {
            return;
        }
        auto& shdDesc  = shaderingDescVec[shadingEt.shadingDescId];
        shdDesc.color  = desc.color;
        entity.visible = desc.visible;
        printf("modifyDstFunc() etId: %d, actDesc: %s, color: 0x%x\n", etId, actDesc.c_str(), shdDesc.color);
    };

    auto   key = static_cast<uint8_t>(status);
    auto&& tar = srcNode.tars[key];
    printf("InteractionSourceSystem::execActToDsiplay(), tar.targets.size(): %zu\n", tar.targets.size());
    for (auto& dst : tar.targets)
    {
        modifyDstFunc(dst, actDesc);
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
}


}