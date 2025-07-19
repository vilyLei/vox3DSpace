#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H


#include <vector>
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"

namespace Voxol::Motion
{

class RenderCmdWorld
{
public:
    std::vector<uint32_t>        commands{};
    std::vector<DrawCmdTestNode> cmdNodes{};
    Voxol::Math::Mat33 projMat{};

    std::vector<uint8_t> buffer{};

    bool dirty = true;

    int ctxWidth = 512;
    int ctxHeight = 512;

public:
    RenderCmdWorld(/* args */);
    ~RenderCmdWorld();

public:
    void           initialize();
    void           run();
    void           setGPUCtxSize(int w, int h);
    void           setMouseXY(float x, float y);
    const uint8_t* cmdBuffer() const;

private:
    bool     mInit = true;
    uint32_t mHeadData[2]{0xffffffff, 0xffffffff};
    uint32_t mTailData[2]{0x0, 0x0};
};

} // namespace Voxol::Render
#endif
