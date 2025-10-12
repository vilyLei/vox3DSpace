#ifndef VOXOL_TILE_SCENE_H
#define VOXOL_TILE_SCENE_H

#include "GridDef.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/DrawCtx.h"
#include "../Test/OglResUtils.h"
#include "../Render/OglFbo.h"
#include <vector>


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
        Render::OglFbo mFbo{};

        int gridSize = 256;

        std::vector<Grid::Unit> gridUnits{};
        Grid::Unit outlineUnit{};
    };
    
}
#endif