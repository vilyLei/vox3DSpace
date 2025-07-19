#include "RenderCmdWorld.h"
#include <cstdio>
namespace Voxol::Render
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

    auto rn    = 10;
    auto cn    = 10;
    auto total = rn * cn;
    auto rsize = 32;

    buffer.resize(total * 2 * sizeof(Mat33) + 32);
    // build head data
    auto bytesTotal = sizeof(mHeadData);
    std::memcpy(buffer.data(), mHeadData, bytesTotal);

    commands.resize(total);
    cmdNodes.resize(total);
    // objTransforms.resize(total);

    auto index = 0;
    for (auto i = 0; i < rn; ++i)
    {
        auto py = 50 + (i * (rsize + 1));
        for (auto j = 0; j < cn; ++j)
        {
            auto px              = 50 + (j * (rsize + 1));
            commands[index]      = 1;
            // objTransforms[index] = Mat33(px, py, rsize, rsize);
            auto& node = cmdNodes[index];
            node.color = 0xff0000aa | ((index) << 8);
            // printf("node.color: %X\n", node.color);
            node.x = px;
            node.y = py;
            node.scaleX = rsize;
            node.scaleY = rsize;

            index++;
        }
    }

    // commands[0] = 3;
    // commands[1] = 5;
    // objTransforms[0] = Mat33(500, 200, 150, 150);
    // objTransforms[1] = Mat33(600, 300, 200, 270);

    mInit = false;
}

void RenderCmdWorld::setGPUCtxSize(int w, int h)
{
    projMat.ortho(w, h);
    dirty = true;
}

void RenderCmdWorld::setMouseXY(float x, float y)
{
    using namespace Voxol::Math;
    // objTransforms[0] = Mat33(x, y, 150, 150);
    auto& node = cmdNodes[0];
    node.x = x;
    node.y = x;
    dirty            = true;
}
void RenderCmdWorld::run()
{
    if (!dirty)
        return;
    dirty = false;


    initialize();


    // auto total = objTransforms.size();
    auto cmdsTotal = cmdNodes.size();

    printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    printf("RenderCmdWorld::run() cmdsTotal: %zu\n", cmdsTotal);

    uint32_t default_cmd = 0x33;
    uint32_t end_cmd = 0x0;
    /// 8bytes head
    /// 4bytes version
    /// 4bytes cmds total
    auto bufBytesLength = buffer.size() - 8;
    auto     bufIndex    = 0;
    auto descSize    = sizeof(mHeadData);
    auto bufPtr = (uint8_t*)buffer.data();
    std::memcpy(bufPtr + bufIndex, mHeadData, descSize);
    bufIndex += descSize;
    uint32_t version = 1;

    printf("version: %d\n", version);

    descSize    = sizeof(version);
    std::memcpy(bufPtr + bufIndex, &version, descSize);
    bufIndex += descSize;
    descSize    = sizeof(cmdsTotal);
    std::memcpy(bufPtr + bufIndex, &cmdsTotal, descSize);
    bufIndex += descSize;

    // for (auto i = 0; i < total; i++)
    // {
    //     transforms[i] = objTransforms[i];
    //     transforms[i].prepend(projMat);
    // }
    //cmdNodes

    RectDrawCmdDesc rectDesc{};
    for (auto i = 0; i < cmdsTotal; i++) {
        
        auto descSize = sizeof(RectDrawCmdDesc);
        if((bufIndex + descSize) > bufBytesLength)
            break;
        auto& node = cmdNodes[i];
        rectDesc.rcmd = node.rcmd;
        rectDesc.color = node.color;
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
    // return (uint8_t*)transforms.data();
    return buffer.data();
}

} // namespace Voxol::Render