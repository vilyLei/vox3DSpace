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

    mInit = false;
}

void RenderCmdWorld::run()
{

    if (!isDirty())
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
bool RenderCmdWorld::isDirty() const
{
    return dirty || nodeScene.dirty;
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