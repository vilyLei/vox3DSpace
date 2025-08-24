#ifndef VOXOL_RENDER_CMD_BUF_BUILDER_H
#define VOXOL_RENDER_CMD_BUF_BUILDER_H

#include <cstdlib>
#include <vector>
#include "RenderCmdComp.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{

class RenderCmdBufBuilder
{
public:
    RenderCmdBufBuilder()  = default;
    ~RenderCmdBufBuilder() = default;

public:
    void           initialize(size_t bufSize);
    void           build(const std::vector<DrawCmdTestNode>& cmdNodes);
    const uint8_t* getBufferPtr() const;

public:
    CameraCmdDesc camDesc{};

private:
    void writeHead();
    void writeVersion();

private:
    uint8_t*             bufPtr;
    int                  version            = 3;
    size_t               bufBytesIndex      = 0;
    size_t               bufBytesSafeLength = 0;
    std::vector<uint8_t> mBuffer{};
    uint32_t             mHeadData[2]{0xffffffff, 0xffffffff};
    uint32_t             mTailData[2]{0x0, 0x0};
};
} // namespace Voxol::Motion
#endif