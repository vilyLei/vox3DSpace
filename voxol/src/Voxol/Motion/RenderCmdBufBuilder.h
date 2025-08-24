#ifndef VOXOL_RENDER_CMD_BUF_BUILDER_H
#define VOXOL_RENDER_CMD_BUF_BUILDER_H

#include <cstdlib>

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
};
} // namespace Voxol::Motion
#endif