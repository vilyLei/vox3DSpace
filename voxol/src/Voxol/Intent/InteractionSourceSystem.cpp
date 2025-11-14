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
    using namespace Render;
    using namespace Interaction;

    InteractionSource srcNode;
    srcNode.id = ID::KeyUint64::make(4);

    // 模拟SDL输入后得到的初始化,
    // ID::KeyUint64::make(1) 表示的是场景层次节点1模拟按钮, 即 entity(1)
    // ID::KeyUint64::make(5) 表示的是场景层次节点5模拟一个面板, 即 entity(5)

    auto             btnID   = ID::KeyUint64::make(3);
    auto             panelID   = ID::KeyUint64::make(5);

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
void InteractionSourceSystem::updateSourceAct(const Render::ID::KeyUint64& srcId, const std::string& actDesc)
{
    for (auto& item : srcMap)
    {
        singalParse(item.second, srcId == item.first, actDesc);
    }
}

void InteractionSourceSystem::execActToDsiplay(Interaction::InteractionSource& srcNode, Interaction::MouseStatus status, std::string actDesc)
{

    auto modifyDstFunc = [this](const Interaction::InteractionTargetDesc& desc, std::string actDesc) {

        if (!compStorage)
            return;

        auto etId = desc.id.protoId();

        auto&& entity            = compStorage->entitiesPool[etId];
        auto&  shaderingEntities = compStorage->shaderingEntitiesPool;
        auto&  shaderingDescVec  = compStorage->shaderingDescPool;

        if (Render::ID::isInvalidID(entity.shadingId))
            return;

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


} // namespace Voxol::Intent