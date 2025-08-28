#include "RenderCmdWorld.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

// void testRotationOp(DrawCmdTestNode& node)
// {
//     Vec2  localPivot{64.0f, 64.0f};
//     Vec2  fixCV{300.0f, 300.0f};
//     auto& desc   = node.drcDesc;
//     auto& bounds = node.drcDesc.bounds;
//     Mat33Utils::makeRotationMat33WithPivot(desc.transform, localPivot, fixCV, bounds.width, bounds.height, node.rotation);
//     node.rotation += 0.1f;
//     node.dirty = false;
// }
void RenderCmdWorld::initialize()
{
    if (!mInit)
        return;

    nodeScene.initialize();
    bufBuilder.initialize((nodeScene.getNodesTotal() + 8) * sizeof(DrawingCmdDesc));
    /*
    using namespace Voxol::Math;


    // printf("RenderCmdWorld::initialize() viewMat:\n");

    auto cmdsTotal = 6;

    auto rn             = 3 * 1;
    auto cn             = 4 * 1;
    auto cmdsBatchTotal = rn * cn;
    auto rsize          = 70.0f;

    auto total = cmdsBatchTotal + cmdsTotal;
    printf("Voxol::Motion::RenderCmdWorld::initialize() total: %d\n", total);

    bufBuilder.initialize((total + 8) * sizeof(DrawingCmdDesc));

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
            desc.bounds.setXY(px, py);
            desc.bounds.setSize(rsize, rsize);
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
        node0.drcDesc.bounds = {{300, 300}, 128, 128};
        node0.drcDesc.mroid  = 2;
        node0.rotation       = 0.3f;
        testRotationOp(node0);

        nodeIndex++;
        auto& node1          = cmdNodes[nodeIndex];
        node1.drcDesc.color  = 0xffffffff;
        node1.drcDesc.bounds = {{500, 350}, 128, 128};
        node1.drcDesc.mroid  = 3;

        nodeIndex++;
        auto& node2          = cmdNodes[nodeIndex];
        node2.drcDesc.color  = 0xff003333;
        node2.drcDesc.bounds = {{300, 530}, 128, 128};
        node2.drcDesc.mroid  = 2;
        nodeIndex++;
        for (auto i = 0; i < 3; ++i)
        {
            auto& drcDesc  = cmdNodes[nodeIndex].drcDesc;
            drcDesc.color  = 0xffbbbb00;
            drcDesc.bounds = {{300.0f, 560.0f + i * 20}, 128, 1};
            drcDesc.mroid  = 2;
            nodeIndex++;
        }
    }
    //*/
    mInit = false;
}

void RenderCmdWorld::run()
{

    if (!dirty && nodeScene.dirty)
        return;
    dirty = false;


    initialize();

    // auto cmdsTotal = static_cast<uint32_t>(nodeScene.cmdBatchNodes.size());

    // RectTarget::Rect boundary = {0, 0, canvas.size.width * 1.0f, canvas.size.height * 1.0f};

    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdBatchNodes[i];
    //     node.moveingNode.update(2, boundary);
    // }
    // for (size_t i = 0; i < cmdsTotal; ++i)
    // {
    //     auto& node0 = cmdBatchNodes[i];
    //     for (size_t j = i + 1; j < cmdsTotal; ++j)
    //     {
    //         auto& node1 = cmdBatchNodes[j];
    //         RectTarget::handleCollision(node0.moveingNode, node1.moveingNode);
    //     }
    // }
    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdBatchNodes[i];
    //     auto& r = node.moveingNode.rect;
    //     node.dirty = true;
    //     node.x = r.pos.x;
    //     node.y = r.pos.y;
    //     //printf("node (x=%f, y=%f)\n", node.x, node.y);
    // }

    nodeScene.run();

    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     nodeScene.cmdBatchNodes[i].update();
    // }
    // cmdsTotal = static_cast<uint32_t>(nodeScene.cmdNodes.size());
    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     nodeScene.cmdNodes[i].update();
    // }

    auto& view      = canvas.view;
    auto& camDesc   = bufBuilder.camDesc;
    camDesc.projMat = view.projMat;
    camDesc.viewMat = view.viewMat;
    bufBuilder.build(nodeScene.cmdBatchNodes, nodeScene.cmdNodes);

    // printf("RenderCmdWorld::run() B cmdsTotal: %zu\n", cmdsTotal);
}

void RenderCmdWorld::update()
{
    nodeScene.update();
}


void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    auto&    view = canvas.view;
    SizeDesc desc{static_cast<float>(w), static_cast<float>(h)};
    if (!canvas.size.isEqual(desc))
    {
        canvas.size = desc;
        view.projMat.ortho(w, h);
        dirty = true;
        // printf("RenderCmdWorld::setGPUCtxSize() ...\n");
    }
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    canvas.view.mousePos = {x, y};
}

void RenderCmdWorld::setMouseParams(const UIMouseParam& param)
{
    dirty = dirty || mouseCtrl.setMouseParams(canvas.view, param);
}

const uint8_t* RenderCmdWorld::cmdBuffer() const
{
    return bufBuilder.getBufferPtr();
}

} // namespace Voxol::Motion