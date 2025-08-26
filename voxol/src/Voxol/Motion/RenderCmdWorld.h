#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"
#include "RenderCmdBufBuilder.h"

namespace Voxol::Motion
{
    
class RenderCmdWorld
{
public:

    std::vector<uint32_t>        commands{};

    std::vector<DrawCmdTestNode> cmdBatchNodes{};
    std::vector<DrawCmdTestNode> cmdNodes{};

    Voxol::Math::Mat33           viewMat{};
    Voxol::Math::Mat33           projMat{};

    CanvasDesc  canvas{};

    RenderCmdBufBuilder bufBuilder{};

    bool dirty = true;

public:
    RenderCmdWorld();
    ~RenderCmdWorld();

public:
    void           initialize();
    void           update();
    void           run();
    void           setGPUCtxSize(int w, int h);
    void           setMouseXY(float x, float y);
    void           setMouseParams(float x, float y, int type, float value);
    const uint8_t* cmdBuffer() const;

private:
    bool     mInit = true;
};

} // namespace Voxol::Motion
#endif