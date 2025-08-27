#include "RenderCmdWorld.h"
#include <cstdio>
#include <cmath>
namespace Voxol::Motion
{

RenderCmdWorld::RenderCmdWorld()
{
}
RenderCmdWorld::~RenderCmdWorld()
{
}
void RenderCmdWorld::initialize()
{
    if (!mInit)
        return;
    using namespace Voxol::Math;


    // printf("RenderCmdWorld::initialize() viewMat:\n");

    auto cmdsTotal = 2;

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

            /*
            auto pindex = i * cn + j;
            if(pindex == 11) {
                node.scaleY = 1;
                node.x = px - (rsize + dis);
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }
            if(pindex == 10) {
                node.scaleX = rsize * 2;
                node.scaleY = 1;
                // node.x = px - (rsize + dis);
                node.y += (rsize + dis) * 2;
                node.rotation = 0.5f;
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }            
            if(pindex == 9) {
                // node.scaleX = rsize * 2;
                node.scaleY = 0.2f;
                // node.x = px - (rsize + dis);
                node.y += (rsize + dis);
                // node.rotation = 0.5f;
                // node.y = py - 0.25f;
                node.color = 0xffaa0000;
            }
            //*/

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

        nodeIndex++;
        auto& node1          = cmdNodes[nodeIndex];
        node1.drcDesc.color  = 0xffffffff;
        node1.drcDesc.bounds = {{500, 350}, 128, 128};
        node1.drcDesc.mroid  = 3;
    }

    mInit = false;
}

void RenderCmdWorld::run()
{
    if (!dirty)
        return;
    dirty = false;


    initialize();

    auto cmdsTotal = static_cast<uint32_t>(cmdBatchNodes.size());

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

    for (auto i = 0; i < cmdsTotal; i++)
    {
        cmdBatchNodes[i].update();
    }
    cmdsTotal = static_cast<uint32_t>(cmdNodes.size());
    for (auto i = 0; i < cmdsTotal; i++)
    {
        cmdNodes[i].update();
    }

    auto& view      = canvas.view;
    auto& camDesc   = bufBuilder.camDesc;
    camDesc.projMat = view.projMat;
    camDesc.viewMat = view.viewMat;
    bufBuilder.build(cmdBatchNodes, cmdNodes);

    // printf("RenderCmdWorld::run() B cmdsTotal: %zu\n", cmdsTotal);
}

void RenderCmdWorld::update()
{
    // //for test
    // if (!cmdNodes.empty())
    // {
    //     auto& node = cmdNodes[0];

    //     Vec2  localPivot{64.0f, 64.0f};
    //     Vec2  fixCV{300.0f, 300.0f};
    //     auto& desc   = node.drcDesc;
    //     auto& bounds = node.drcDesc.bounds;
    //     Mat33Utils::makeRotationMat33WithPivot(desc.transform, localPivot, fixCV, bounds.width, bounds.height, node.rotation);
    //     node.rotation += 0.1f;
    //     node.dirty = false;
    //     dirty      = true;
    // }
}


void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    auto& view      = canvas.view;
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