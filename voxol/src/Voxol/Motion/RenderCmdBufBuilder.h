#ifndef VOXOL_RENDER_CMD_BUF_BUILDER_H
#define VOXOL_RENDER_CMD_BUF_BUILDER_H

#include <cstdlib>
#include <vector>
#include "RenderCmdComp.h"

namespace Voxol::Motion
{

class RenderCmdBufBuilder
{
public:
    RenderCmdBufBuilder()  = default;
    ~RenderCmdBufBuilder() = default;

public:
    void initialize(size_t bufSize);
    void build();

private:
    std::vector<uint8_t> mBuffer{};
    uint32_t mHeadData[2]{0xffffffff, 0xffffffff};
    uint32_t mTailData[2]{0x0, 0x0};
};
} // namespace Voxol::Motion
#endif