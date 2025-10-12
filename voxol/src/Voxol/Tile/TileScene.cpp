#include "TileScene.h"
#include "../Math/MathDef.h"
namespace Voxol::Tile
{
    
    void TileScene::initalize()
    {
        mFbo.init(GL_ZERO);

        outlineUnit.drawUnit.vertex.toLine();
        Test::Gpu::buildBaseDrawUnit(outlineUnit.drawUnit);
        
    }
    
    void TileScene::run(const Render::Draw::DrawContext& ctx)
    {
        auto&       params = ctx.drawParam;
        Math::Mat33 vpM = params.projMat;
        vpM.append(params.viewMat);
        ctx.drawCall({}, vpM);

        // the default gridSize value is 256
        auto value = gridSize * ctx.zoom;
        auto lv    = Math::calcCeilOfTwoLevelToInt(value);

        auto dstSize = std::pow(2, lv);
        auto lvScale = float(gridSize) / dstSize;
        if (ctx.dirty)
        {
            printf("value: %f, lv: %d, dstSize: %f, lvScale: %f\n", value, lv, dstSize, lvScale);
        }


        auto currGridSize = 256.0f;
        auto texSize      = gridSize;

        currGridSize = gridSize * lvScale;


        auto pv = RC::xyToRC(3, 0, 10);

        //outlineUnit.objMat.setTo(0, 0, currGridSize, currGridSize);
        outlineUnit.drawUnit.setColor(0x50550055);

        auto k = 0;
        auto gr = RC::xyRectToRCRect(params.viewWBounds, currGridSize);
        /*
        for (auto r = gr.minR; r <= gr.maxR; r++)
        {
            for (auto c = gr.minC; c <= gr.maxC; c++)
            {
                gridUnits[k].setPosAndSize({r, c}, currGridSize);
                buildGridUnit(gridUnits[k], ctx, texSize);

                k++;
            }
        }

        for (auto i = 0; i < k; i++)
        {
            gridUnits[i].drawUnit.mvp = vpM;
            gridUnits[i].drawUnit.draw();
        }
        //*/

        auto& drawUnit = outlineUnit.drawUnit;
        for (auto r = gr.minR; r <= gr.maxR; r++)
        {
            for (auto c = gr.minC; c <= gr.maxC; c++)
            {
                outlineUnit.setRCAndAreaSize({r, c}, currGridSize);
                outlineUnit.drawUnit.mvp = vpM;
                outlineUnit.drawUnit.draw();
            }
        }
    }
}