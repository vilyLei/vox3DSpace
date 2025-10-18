#include "TileSystem.h"

namespace Voxol::Tile
{

void TileSystem::initalize()
{
    mScene.initalize();
}

void TileSystem::addDirtyBounds(const Math::Bounds& bounds, int phase)
{
    mScene.addDirtyBounds(bounds, phase);
}
void TileSystem::run(const Render::Draw::DrawContext& ctx)
{
    mScene.run(ctx);
}
} // namespace Voxol::Tile