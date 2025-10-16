#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"
#include "RenderCmdBufBuilder.h"
#include "../System/UIMouseCtrl.h"
#include "RenderNodeScene.h"

namespace Voxol::Motion
{

class RenderCmdWorld
{
public:
    // std::vector<uint32_t>        commands{};
    // std::vector<DrawCmdTestNode> cmdBatchNodes{};
    // std::vector<DrawCmdTestNode> cmdNodes{};


    CanvasDesc          canvas{};
    RenderCmdBufBuilder bufBuilder{};

    RenderNodeScene nodeScene{};
    System::UIMouseCtrl mouseCtrl{};

public:
    RenderCmdWorld()  = default;
    ~RenderCmdWorld() = default;

public:
    void           initialize();
    void           update();
    void           run();
    bool           isDirty() const;
    void           setGPUCtxSize(int w, int h);
    void           setMouseXY(float x, float y);
    void           setMouseParams(const System::UIMouseParam& param);
    const uint8_t* cmdBuffer() const;

private:
    bool mInit = true;
    bool dirty = true;
};

} // namespace Voxol::Motion
#endif