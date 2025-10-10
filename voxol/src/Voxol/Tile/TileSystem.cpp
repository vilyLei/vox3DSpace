#include "TileSystem.h"
namespace Voxol::Tile
{

void TileSystem::initalize()
{
    mScene.initalize();
}

void TileSystem::run(const Render::Draw::DrawContext& ctx)
{
    mScene.run(ctx);
}
} // namespace Voxol::Tile