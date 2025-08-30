#include "RenderNodeScene.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

void testRotationOp(DrawCmdTestNode& node)
{
    Vec2  localPivot{64.0f, 64.0f};
    Vec2  fixCV{300.0f, 300.0f};
    auto& desc   = node.drcDesc;
    auto& bounds = node.drcDesc.bounds;
    Mat33Utils::makeRotationMat33WithPivot(desc.transform, localPivot, fixCV, bounds.width, bounds.height, node.rotation);
    node.rotation += 0.1f;
    node.dirty = false;
}
void RenderNodeScene::initialize()
{
    if (!mInit)
        return;
    using namespace Voxol::Math;


    // printf("RenderNodeScene::initialize() ...\n");

    auto cmdsTotal = 6;

    auto rn             = 3 * 1;
    auto cn             = 4 * 1;
    auto cmdsBatchTotal = rn * cn;
    auto rsize          = 70.0f;

    auto total = cmdsBatchTotal + cmdsTotal;
    printf("Voxol::Motion::RenderNodeScene::initialize() total: %d\n", total);

    commands.resize(total);
    cmdBatchNodes.resize(cmdsBatchTotal);
    cmdNodes.resize(cmdsTotal);

    auto  index = 0;
    float dis   = 2;
    for (auto i = 0; i < rn; ++i)
    {
        auto py = 50 + (i * (rsize + dis));
        for (auto j = 0; j < cn; ++j)
        {
            auto px         = 50 + (j * (rsize + dis));
            commands[index] = 1;
            auto& node      = cmdBatchNodes[index];
            auto& desc      = node.drcDesc;
            desc.color      = 0xff0000aa | ((index % 256) << 16);
            desc.color      = desc.color | (((i * j * 2) % 256) << 16);
            // printf("node.color: %X\n", node.color);
            node.bounds.setXY(px, py);
            node.bounds.setSize(rsize, rsize);
            desc.bounds = node.bounds;
            desc.mroid = 1;

            node.moveingNode.rect = desc.bounds;
            node.init();

            index++;
        }
    }

    if (!cmdNodes.empty())
    {
        auto  nodeIndex      = 0;
        auto& node0          = cmdNodes[nodeIndex];
        node0.drcDesc.color  = 0xff00aaaa;
        node0.bounds = {{300, 300}, 128, 128};
        node0.drcDesc.bounds = node0.bounds;
        node0.drcDesc.mroid  = 2;
        node0.rotation       = 0.3f;
        testRotationOp(node0);        
        node0.drcDesc.bounds.outset(50, 50);
        //
        nodeIndex++;
        auto& node1          = cmdNodes[nodeIndex];
        node1.drcDesc.color  = 0xffffffff;
        node1.bounds = {{500, 350}, 128, 128};
        node1.drcDesc.bounds = node1.bounds;
        node1.drcDesc.mroid  = 3;

        nodeIndex++;
        auto& node2          = cmdNodes[nodeIndex];
        node2.drcDesc.color  = 0xff003333;
        node2.bounds = {{300, 530}, 128, 128};
        node2.drcDesc.bounds = node2.bounds;
        node2.drcDesc.mroid  = 2;
        nodeIndex++;
        for (auto i = 0; i < 3; ++i)
        {
            auto& drcDesc  = cmdNodes[nodeIndex].drcDesc;
            drcDesc.color  = 0xffbbbb00;
            cmdNodes[nodeIndex].bounds = {{300.0f, 560.0f + i * 20}, 128, 1};
            drcDesc.bounds = cmdNodes[nodeIndex].bounds;
            drcDesc.mroid  = 2;
            nodeIndex++;
        }
        cmdNodes[nodeIndex - 1].rotation = -0.2f;
        cmdNodes[nodeIndex - 1].drcDesc.bounds.outset(20, 50);
    }

    mInit = false;
}
void RenderNodeScene::update()
{
    // if (!cmdNodes.empty())
    // {
    //     testRotationOp(cmdNodes[0]);
    //     cmdNodes[0].rotation += 0.05f;
    //     dirty = true;
    // }
}
void RenderNodeScene::run()
{
    if (!dirty)
        return;
    dirty = false;

    auto cmdsTotal = cmdBatchNodes.size();
    for (auto i = 0; i < cmdsTotal; i++)
    {
        cmdBatchNodes[i].update();
    }
    cmdsTotal = cmdNodes.size();
    for (auto i = 0; i < cmdsTotal; i++)
    {
        cmdNodes[i].update();
    }
}
int RenderNodeScene::getNodesTotal() const
{
    return static_cast<int>(cmdBatchNodes.size() + cmdNodes.size());
}
} // namespace Voxol::Motion