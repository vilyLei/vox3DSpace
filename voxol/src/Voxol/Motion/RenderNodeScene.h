#ifndef VOXOL_RENDER_NODE_SCENE_H
#define VOXOL_RENDER_NODE_SCENE_H

#include "RenderCmdComp.h"
#include "../Math/Mat33.h"
#include "RenderCmdNode.h"
#include "RenderCmdBufBuilder.h"
#include "UIMouseCtrl.h"

namespace Voxol::Motion
{

class RenderNodeScene
{
public:
    std::vector<uint32_t> commands{};

    std::vector<DrawCmdTestNode> cmdBatchNodes{};
    std::vector<DrawCmdTestNode> cmdNodes{};


    bool dirty = true;

public:
    RenderNodeScene()  = default;
    ~RenderNodeScene() = default;

public:
    void initialize();
    int getNodesTotal() const;

private:
    bool mInit = true;
};

} // namespace Voxol::Motion
#endif