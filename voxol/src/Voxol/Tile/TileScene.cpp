#include "TileScene.h"
#include "../Math/MathDef.h"
namespace Voxol::Tile
{
    
    void TileScene::initalize()
    {
        mFbo.init(GL_ZERO);
    }
    
    void TileScene::run(const Render::Draw::DrawContext& ctx)
    {
        auto&       params = ctx.drawParam;
        Math::Mat33 vpM = params.projMat;
        vpM.append(params.viewMat);
        ctx.drawCall({}, vpM);

        /*
        // the default gridSize value is 256
        auto value = gridSize * ctx.zoom;
        auto lv    = Math::calcCeilOfTwoLevelToInt(value);

        auto dstSize = std::pow(2, lv);
        auto lvScale = float(gridSize) / dstSize;
        if (ctx.dirty)
        {
            printf("value: %f, lv: %d, dstSize: %f, lvScale: %f\n", value, lv, dstSize, lvScale);
        }

        auto& rparams = ctx.drawParam;
        auto  vM      = rparams.viewMat;
        vpM           = rparams.projMat;
        vpM.append(vM);

        auto currGridSize = 256.0f;
        auto texSize      = gridSize;

        vpM = rparams.projMat;
        vpM.append(rparams.viewMat);

        currGridSize = gridSize * lvScale;
        gridOutlineUnit.objMat.setTo(0, 0, currGridSize, currGridSize);
        gridOutlineUnit.setColor(0x50550055);

        auto k = 0;

        auto gr = RC::calcRCRange(rparams.viewWBounds, currGridSize);
        for (auto r = gr.minR; r <= gr.maxR; r++)
        {
            for (auto c = gr.minC; c <= gr.maxC; c++)
            {
                gridUnits[k].setPosAndSize({r, c}, currGridSize);
                buildGridUnit(gridUnits[k], rctx, texSize);

                k++;
            }
        }

        for (auto i = 0; i < k; i++)
        {
            gridUnits[i].drawUnit.mvp = vpM;
            gridUnits[i].drawUnit.draw();
        }
        //*/
    }
}