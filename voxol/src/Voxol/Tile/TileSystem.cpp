#include "TileSystem.h"

namespace Voxol::Tile
{

void TileSystem::initalize()
{
    mScene.initalize();
}

void TileSystem::addDirtyBounds(const Math::Bounds& bounds)
{
    mScene.addDirtyBounds(bounds);
}
void TileSystem::run(const Render::Draw::DrawContext& ctx)
{
    mScene.run(ctx);
}
} // namespace Voxol::Tile