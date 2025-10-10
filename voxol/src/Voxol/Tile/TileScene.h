#ifndef VOXOL_TILE_SCENE_H
#define VOXOL_TILE_SCENE_H

#include "GridDef.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/DrawCtx.h"
#include "../Test/OglResUtils.h"
#include "../Test/OglFbo.h"


namespace Voxol::Tile
{
    using namespace Voxol::Math;


    class TileScene
    {
    public:
        TileScene() = default;
        ~TileScene() = default;
    public:
    
        void initalize();        
        void run(const Render::Draw::DrawContext& ctx);

    private:
        Test::OglFbo mFbo{};
    };
    
}
#endif