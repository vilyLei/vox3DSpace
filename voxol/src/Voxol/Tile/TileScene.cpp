#include "TileScene.h"
namespace Voxol::Tile
{
    
    void TileScene::initalize()
    {
        mFbo.init(GL_ZERO);
    }
    
    void TileScene::run(const Render::Draw::DrawContext& ctx)
    {
        auto&       params = ctx.params;
        Math::Mat33 vpM = params.projMat;
        vpM.append(params.viewMat);
        ctx.drawCall({}, vpM);
    }
}