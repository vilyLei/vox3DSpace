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
    public:
        std::vector<uint32_t> commands{};
        std::vector<Voxol::Math::Mat33> objTransforms{};
        std::vector<Voxol::Math::Mat33> transforms{};
        Voxol::Math::Mat33 projMat{};
        bool dirty = true;

    public:
        RenderCmdWorld(/* args */);
        ~RenderCmdWorld();

    public:
        void initialize();
        void run();

    };
    
}
#endif
