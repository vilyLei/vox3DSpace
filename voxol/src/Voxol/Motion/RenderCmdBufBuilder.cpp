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
}

void RenderCmdBufBuilder::writeHead()
{
    bufBytesIndex      = 0;
    auto descBytesSize = sizeof(mHeadData);
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

void RenderCmdBufBuilder::writeRenderingBegin()
{
    auto trunkCmd      = 21;
    auto descBytesSize = sizeof(trunkCmd);
    std::memcpy(bufPtr + bufBytesIndex, &trunkCmd, descBytesSize);
    bufBytesIndex += descBytesSize;
}
void RenderCmdBufBuilder::writeBatchCmdNodeBegin(uint32_t cmdsTotal)
{
    // printf("RenderCmdBufBuilder::writeBatchCmdNodeBegin(), cmdsTotal: %d\n", cmdsTotal);
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

void RenderCmdBufBuilder::writeCmdNode(const DrawCmdTestNode& node)
{
    // unitDesc.rcmd  = node.rcmd;
    // unitDesc.mroid = node.mroid;
    // unitDesc.color = node.color;
    // auto& bounds   = unitDesc.bounds;
    // bounds.pos.x   = node.x;
    // bounds.pos.y   = node.y;
    // bounds.width   = node.scaleX;
    // bounds.height  = node.scaleY;

    // unitDesc.transform = node.transform;

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


    node.drcDesc.updateToBuffer(bufPtr + bufBytesIndex, bufBytesIndex, bufBytesSafeLength);

    bufBytesIndex += node.drcDesc.descSize * 4;
}
void RenderCmdBufBuilder::build(const std::vector<DrawCmdTestNode>& cmdBatchNodes, const std::vector<DrawCmdTestNode>& cmdNodes)
{

    // printf("RenderCmdBufBuilder::build() sizeof(projMat): %zu\n", sizeof(projMat));
    // printf("RenderCmdBufBuilder::build() cmdsTotal: %zu\n", cmdsTotal);

    writeHead();
    writeVersion();
    writeCamInfo();    
    writeRenderingBegin();

    DrawingCmdDesc unitDesc{};

    if (!cmdBatchNodes.empty())
    {
        auto tot = static_cast<uint32_t>(cmdBatchNodes.size());
        writeBatchCmdNodeBegin(tot);
        for (auto i = 0; i < tot; i++)
        {
            writeCmdNode(cmdBatchNodes[i]);
        }
    }
    if (!cmdNodes.empty())
    {
        auto tot = static_cast<uint32_t>(cmdNodes.size());
        for (auto i = 0; i < tot; i++)
        {
            writeCmdNode(cmdNodes[i]);
        }
    }


    writeTail();

    // printf("RenderCmdBufBuilder::build() B cmdsTotal: %zu\n", cmdsTotal);
}

const uint8_t* RenderCmdBufBuilder::getBufferPtr() const
{
    return mBuffer.data();
}
} // namespace Voxol::Motion