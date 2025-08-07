#include "RenderCmdWorld.h"
#include <cstdio>
namespace Voxol::Motion
{
RenderCmdWorld::RenderCmdWorld(/* args */)
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

    printf("Voxol::Motion::RenderCmdWorld::initialize() ...\n");

    auto rn    = 16;
    auto cn    = 16;
    auto total = rn * cn;
    auto rsize = 32.0f;

    buffer.resize(total * 2 * sizeof(Mat33) + 32);
    // build head data
    auto bytesTotal = sizeof(mHeadData);
    std::memcpy(buffer.data(), mHeadData, bytesTotal);

    commands.resize(total);
    cmdNodes.resize(total);

    auto index = 0;
    auto dis = 5;
    for (auto i = 0; i < rn; ++i)
    {
        auto py = 50 + (i * (rsize + dis));
        for (auto j = 0; j < cn; ++j)
        {
            auto px         = 50 + (j * (rsize + dis));
            commands[index] = 1;
            // objTransforms[index] = Mat33(px, py, rsize, rsize);
            auto& node = cmdNodes[index];
            node.color = 0xff0000aa | ((index) << 16);
            node.color = node.color | ((i * j * 2) << 16);
            // printf("node.color: %X\n", node.color);
            node.x                = px;
            node.y                = py;
            node.scaleX           = rsize;
            node.scaleY           = rsize;
            node.moveingNode.rect = {node.x, node.y, rsize * 1.0f, rsize * 1.0f};
            node.init();

            index++;
        }
    }

    mInit = false;
}

void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    projMat.ortho(w, h);

    ctxWidth  = w;
    ctxHeight = h;

    dirty = true;
    // printf("RenderCmdWorld::setGPUCtxSize() ...\n");
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    using namespace Voxol::Math;
    
    auto& node = cmdNodes[0];
    node.x     = x;
    node.y     = y;
    dirty      = true;
}
void RenderCmdWorld::run()
{
    if (!dirty)
        return;
    dirty = false;


    initialize();

    auto cmdsTotal = static_cast<uint32_t>(cmdNodes.size());

    // printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    // printf("RenderCmdWorld::run() cmdsTotal: %zu\n", cmdsTotal);

    uint32_t default_cmd = 0x33;
    uint32_t end_cmd     = 0x0;
    /// 8bytes head
    /// 4bytes version
    /// 4bytes cmds total
    auto bufBytesLength = buffer.size() - 8;
    auto bufIndex       = 0;
    auto descSize       = sizeof(mHeadData);
    auto bufPtr         = (uint8_t*)buffer.data();
    std::memcpy(bufPtr + bufIndex, mHeadData, descSize);
    bufIndex += descSize;
    uint32_t version = 1;

    // printf("version: %d\n", version);

    descSize = sizeof(version);
    std::memcpy(bufPtr + bufIndex, &version, descSize);
    bufIndex += descSize;
    descSize = sizeof(cmdsTotal);
    std::memcpy(bufPtr + bufIndex, &cmdsTotal, descSize);
    bufIndex += descSize;


    RectTarget::Rect boundary = {0, 0, ctxWidth * 1.0f, ctxHeight * 1.0f};

    for (auto i = 0; i < cmdsTotal; i++)
    {
        auto& node = cmdNodes[i];
        node.moveingNode.update(2, boundary);
    }
    for (size_t i = 0; i < cmdsTotal; ++i)
    {
        auto& node0 = cmdNodes[i];
        for (size_t j = i + 1; j < cmdsTotal; ++j)
        {
            auto& node1 = cmdNodes[j];
            RectTarget::handleCollision(node0.moveingNode, node1.moveingNode);
        }
    }
    for (auto i = 0; i < cmdsTotal; i++)
    {
        auto& node = cmdNodes[i];
        auto& r = node.moveingNode.rect;
        node.x = r.pos.x;
        node.y = r.pos.y;
        //printf("node (x=%f, y=%f)\n", node.x, node.y);
    }

    RectDrawCmdDesc rectDesc{};
    for (auto i = 0; i < cmdsTotal; i++)
    {

        auto descSize = sizeof(RectDrawCmdDesc);
        if ((bufIndex + descSize) > bufBytesLength)
            break;
        auto& node = cmdNodes[i];
        
        rectDesc.rcmd = node.rcmd;
        rectDesc.color    = node.color;
        node.updateToMat33(rectDesc.transform);

        // rectDesc.transform.print();
        // printf(">    >     >\n");
        rectDesc.transform.prepend(projMat);
        // rectDesc.transform.print();
        // printf(">    >     >\n");

        rectDesc.updateToBuffer(bufPtr + bufIndex);

        bufIndex += descSize;
    }

    auto bytesTotal = sizeof(mTailData);
    std::memcpy(bufPtr + bufIndex, mTailData, bytesTotal);
}


const uint8_t* RenderCmdWorld::cmdBuffer() const
{
    return buffer.data();
}

} // namespace Voxol::Motion