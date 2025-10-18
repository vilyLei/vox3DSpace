#ifndef VOXOL_TILE_SYSTEM_H
#define VOXOL_TILE_SYSTEM_H

#include "TileScene.h"

namespace Voxol::Tile
{
class TileSystem
{
public:
    using SP = std::shared_ptr<TileSystem>;
    using WP = std::weak_ptr<TileSystem>;
    using UP = std::unique_ptr<TileSystem>;
public:
    static TileSystem::SP make();

public:
    TileSystem()  = default;
    ~TileSystem() = default;

public:
    void initalize();
    void addDirtyBounds(const Math::Bounds& bounds, int phase);
    void run(const Render::Draw::DrawContext& ctx);

private:
    TileScene mScene{};
};
} // namespace Voxol::Tile
#endif