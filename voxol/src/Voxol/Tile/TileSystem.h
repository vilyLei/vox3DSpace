#ifndef VOXOL_TILE_SYSTEM_H
#define VOXOL_TILE_SYSTEM_H

#include "TileScene.h"

namespace Voxol::Tile
{
class TileSystem
{
public:
    TileSystem()  = default;
    ~TileSystem() = default;

public:
    void initalize();
    void run();

private:
    TileScene mScene{};
};
} // namespace Voxol::Tile
#endif