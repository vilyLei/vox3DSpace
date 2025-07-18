#ifndef VOXOL_RENDER_CMD_WORLD_H
#define VOXOL_RENDER_CMD_WORLD_H


#include <vector>
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"

namespace Voxol::Render
{
class RenderCmdWorld
{
private:
    bool mInit = true;

    uint32_t mHeadData[2]{0xffffffff, 0xffffffff};

public:
    std::vector<uint32_t>           commands{};
    std::vector<Voxol::Math::Mat33> objTransforms{};
    std::vector<Voxol::Math::Mat33> transforms{};
    Voxol::Math::Mat33              projMat{};

    std::vector<uint8_t> buffer{};

    bool dirty = true;

public:
    RenderCmdWorld(/* args */);
    ~RenderCmdWorld();

public:
    void initialize();
    void run();
    void setGPUCtxSize(int w, int h);
    void setMouseXY(float x, float y);
    const uint8_t* cmdBuffer() const;
};

} // namespace Voxol::Render
#endif
