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
    void addDirtyBounds(const Math::Bounds& bounds);
    void run(const Render::Draw::DrawContext& ctx);

private:
    TileScene mScene{};
};
} // namespace Voxol::Tile
#endif