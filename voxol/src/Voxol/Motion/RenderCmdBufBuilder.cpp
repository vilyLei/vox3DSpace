#include "RenderCmdBufBuilder.h"
namespace Voxol::Motion
{
void RenderCmdBufBuilder::initialize(size_t bufSize)
{
    if (bufSize < 32)
        bufSize = 32;

    mBuffer.resize(bufSize);
    bufBytesSafeLength = bufSize - 8;
    bufBytesIndex      = 0;
    bufPtr             = (uint8_t*)mBuffer.data();


    // build head data
    // auto bytesTotal = sizeof(mHeadData);
    // std::memcpy(mBuffer.data(), mHeadData, bytesTotal);
}

void RenderCmdBufBuilder::writeHead()
{
    bufBytesIndex      = 0;
    auto descBytesSize = sizeof(mHeadData);
    // auto bufPtr        = (uint8_t*)mBuffer.data();
    std::memcpy(bufPtr + bufBytesIndex, mHeadData, descBytesSize);
    bufBytesIndex += descBytesSize;
}
void RenderCmdBufBuilder::writeTail()
{
    auto bytesTotal = sizeof(mTailData);
    std::memcpy(bufPtr + bufBytesIndex, mTailData, bytesTotal);
}
void RenderCmdBufBuilder::writeVersion()
{
    auto descBytesSize = sizeof(version);
    std::memcpy(bufPtr + bufBytesIndex, &version, descBytesSize);
    bufBytesIndex += descBytesSize;
}

void RenderCmdBufBuilder::writeCamInfo()
{
    uint32_t trunkCmd      = 20;
    auto     descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;
    camDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesSafeLength);
    bufBytesIndex += camDesc.descSize * 4;
}
void RenderCmdBufBuilder::writeBatchCmdNodeBegin(uint32_t cmdsTotal)
{
    // batch rounit rendering cmd
    auto trunkCmd      = 22;
    auto descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;

    BatchElementCmdDesc batchDesc{};
    batchDesc.descSize = cmdsTotal;
    batchDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesSafeLength);
    bufBytesIndex += 2 * 4;
}

void RenderCmdBufBuilder::writeCmdNode(RectDrawCmdDesc& unitDesc, const DrawCmdTestNode& node)
{
        unitDesc.rcmd  = node.rcmd;
        unitDesc.color = node.color;
        auto& bounds   = unitDesc.bounds;
        bounds.pos.x   = node.x;
        bounds.pos.y   = node.y;
        bounds.width   = node.scaleX;
        bounds.height  = node.scaleY;

        unitDesc.transform = node.transform;

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

        unitDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesSafeLength);

        bufBytesIndex += unitDesc.descSize * 4;
}
void RenderCmdBufBuilder::build(const std::vector<DrawCmdTestNode>& cmdNodes)
{
    auto cmdsTotal = static_cast<uint32_t>(cmdNodes.size());

    // printf("RenderCmdBufBuilder::build() sizeof(projMat): %zu\n", sizeof(projMat));
    // printf("RenderCmdBufBuilder::build() cmdsTotal: %zu\n", cmdsTotal);

    uint32_t default_cmd = 0x32;
    uint32_t end_cmd     = 0x0;

    writeHead();
    writeVersion();
    writeCamInfo();
    writeBatchCmdNodeBegin(cmdsTotal);

    RectDrawCmdDesc unitDesc{};
    for (auto i = 0; i < cmdsTotal; i++)
    {

        /*
        auto& node = cmdNodes[i];

        unitDesc.rcmd  = node.rcmd;
        unitDesc.color = node.color;
        auto& bounds   = unitDesc.bounds;
        bounds.pos.x   = node.x;
        bounds.pos.y   = node.y;
        bounds.width   = node.scaleX;
        bounds.height  = node.scaleY;
        unitDesc.transform = node.transform;
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
        unitDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesSafeLength);
        bufBytesIndex += unitDesc.descSize * 4;
        //*/
        writeCmdNode(unitDesc, cmdNodes[i]);
    }
    writeTail();

    // printf("RenderCmdBufBuilder::build() B cmdsTotal: %zu\n", cmdsTotal);
}

const uint8_t* RenderCmdBufBuilder::getBufferPtr() const
{
    return mBuffer.data();
}
} // namespace Voxol::Motion