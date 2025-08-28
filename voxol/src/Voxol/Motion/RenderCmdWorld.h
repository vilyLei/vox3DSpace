#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"
#include "RenderCmdBufBuilder.h"
#include "UIMouseCtrl.h"
#include "RenderNodeScene.h"

namespace Voxol::Motion
{
    
class RenderCmdWorld
{
public:

    // std::vector<uint32_t>        commands{};
    // std::vector<DrawCmdTestNode> cmdBatchNodes{};
    // std::vector<DrawCmdTestNode> cmdNodes{};
    

    CanvasDesc  canvas{};
    RenderCmdBufBuilder bufBuilder{};
    
    RenderNodeScene nodeScene{};
    UIMouseCtrl mouseCtrl{};

    bool dirty = true;

public:
    RenderCmdWorld() = default;
    ~RenderCmdWorld() = default;

public:
    void           initialize();
    void           update();
    void           run();
    void           setGPUCtxSize(int w, int h);
    void           setMouseXY(float x, float y);
    void           setMouseParams(const UIMouseParam& param);
    const uint8_t* cmdBuffer() const;

private:
    bool     mInit = true;
};

} // namespace Voxol::Motion
#endif