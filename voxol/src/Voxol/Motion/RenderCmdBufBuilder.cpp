#include "RenderCmdBufBuilder.h"
namespace Voxol::Motion
{
void RenderCmdBufBuilder::initialize(size_t bufSize)
{
    mBuffer.resize(bufSize);
    // build head data
    auto bytesTotal = sizeof(mHeadData);
    std::memcpy(mBuffer.data(), mHeadData, bytesTotal);
}
void RenderCmdBufBuilder::build(const std::vector<DrawCmdTestNode>& cmdNodes, CameraCmdDesc& camDesc)
{
    auto cmdsTotal = static_cast<uint32_t>(cmdNodes.size());

    // printf("RenderCmdWorld::run() sizeof(projMat): %zu\n", sizeof(projMat));
    // printf("RenderCmdWorld::run() cmdsTotal: %zu\n", cmdsTotal);

    uint32_t default_cmd = 0x32;
    uint32_t end_cmd     = 0x0;
    /// 8bytes head
    /// 4bytes version
    /// 4bytes cmds total
    auto bufBytesLength = mBuffer.size() - 8;
    auto bufBytesIndex  = 0;
    auto descBytesSize  = sizeof(mHeadData);
    auto bufPtr         = (uint8_t*)mBuffer.data();
    std::memcpy(bufPtr + bufBytesIndex, mHeadData, descBytesSize);
    bufBytesIndex += descBytesSize;
    uint32_t version = 3;

    // printf("version: %d\n", version);

    descBytesSize = sizeof(version);
    std::memcpy(bufPtr + bufBytesIndex, &version, descBytesSize);
    bufBytesIndex += descBytesSize;

    uint32_t trunkCmd = 20;
    descBytesSize     = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;

    // printf("projMat:\n");
    // projMat.print();

    // CameraCmdDesc camDesc{};
    // camDesc.projMat = projMat;
    // camDesc.viewMat = viewMat;
    camDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);
    bufBytesIndex += camDesc.descSize * 4;
    // batch rounit rendering cmd
    trunkCmd      = 22;
    descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;

    BatchElementCmdDesc batchDesc{};
    batchDesc.descSize = cmdsTotal;
    batchDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);
    bufBytesIndex += 2 * 4;

    // RectTarget::Rect boundary = {0, 0, canvas.size.width * 1.0f, canvas.size.height * 1.0f};

    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdNodes[i];
    //     node.moveingNode.update(2, boundary);
    // }
    // for (size_t i = 0; i < cmdsTotal; ++i)
    // {
    //     auto& node0 = cmdNodes[i];
    //     for (size_t j = i + 1; j < cmdsTotal; ++j)
    //     {
    //         auto& node1 = cmdNodes[j];
    //         RectTarget::handleCollision(node0.moveingNode, node1.moveingNode);
    //     }
    // }
    // for (auto i = 0; i < cmdsTotal; i++)
    // {
    //     auto& node = cmdNodes[i];
    //     auto& r = node.moveingNode.rect;
    //     node.dirty = true;
    //     node.x = r.pos.x;
    //     node.y = r.pos.y;
    //     //printf("node (x=%f, y=%f)\n", node.x, node.y);
    // }

    RectDrawCmdDesc rectDesc{};
    for (auto i = 0; i < cmdsTotal; i++)
    {

        auto& node = cmdNodes[i];

        rectDesc.rcmd  = node.rcmd;
        rectDesc.color = node.color;
        // node.update();
        auto& bounds  = rectDesc.bounds;
        bounds.pos.x  = node.x;
        bounds.pos.y  = node.y;
        bounds.width  = node.scaleX;
        bounds.height = node.scaleY;

        rectDesc.transform = node.transform;

        // rectDesc.transform = projMat;
        // // view mat and proj mat maybe become to a camera function
        // rectDesc.transform.append(viewMat);
        // rectDesc.transform.append(node.transform);

        // node.updateToMat33(rectDesc.transform);
        // rectDesc.transform.print();
        // printf(">    >     >\n");
        // rectDesc.transform.prepend(viewMat);
        // rectDesc.transform.prepend(projMat);
        // rectDesc.transform.print();
        // printf(">    >     >\n");

        rectDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesLength);

        bufBytesIndex += rectDesc.descSize * 4;
    }

    auto bytesTotal = sizeof(mTailData);
    std::memcpy(bufPtr + bufBytesIndex, mTailData, bytesTotal);
}

const uint8_t* RenderCmdBufBuilder::getBufferPtr() const
{
    return mBuffer.data();
}
} // namespace Voxol::Motion