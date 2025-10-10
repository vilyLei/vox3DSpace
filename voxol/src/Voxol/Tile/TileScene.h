#ifndef VOXOL_TILE_SCENE_H
#define VOXOL_TILE_SCENE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Test/OglResUtils.h"

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
        void run();
    };
    
}
#endif