#include "TileScene.h"
#include "../Math/MathDef.h"
namespace Voxol::Tile
{
    
    void TileScene::initalize()
    {
        mFbo.init(GL_ZERO);

        outlineUnit.drawUnit.vertex.toLine();
        Test::Gpu::buildBaseDrawUnit(outlineUnit.drawUnit);

        gridUnits.resize(128);

        Test::Gpu::buildTexDrawUnitWithTex(gridUnits[0].drawUnit, GL_ZERO, true);
        
    }

    void TileScene::buildGrid(Grid::Unit& unit, const Render::Draw::DrawContext& ctx, int fboTexSize)
    {

        auto&  drawUnit = unit.drawUnit;
        auto&& pos      = drawUnit.objMat.getXY();

        auto        scale = fboTexSize / unit.areaSize;

        Math::Mat33 vpM = gridProjMat;

        Math::Mat33 viewM;
        viewM.setScaleXY(scale, scale);
        viewM.setXY(-pos.x * scale, -pos.y * scale);
        vpM.append(viewM);

        auto& rparams = ctx.drawParam;
        auto& vp      = ctx.clearParam.viewport;

        mFbo.bindFBO();
        mFbo.bindTextureAt(drawUnit.getTextureAt(0), 0, gridSize, gridSize);
        mFbo.renderBegin(clearParam);

        ctx.drawCall(rparams.viewWBounds, vpM);
        mFbo.unbindFBO(ctx.clearParam, true);

        Test::Gpu::buildTexDrawUnitWithTex(drawUnit, mFbo.getTextureAt(0), true);
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
            printf("TileScene::run(), value: %f, lv: %d, dstSize: %f, lvScale: %f\n", value, lv, dstSize, lvScale);
        }

        gridProjMat.ortho(gridSize, gridSize);
        clearParam.viewport = {0, 0, gridSize, gridSize};
        clearParam.clearColor = {0.95f, 0.95f, 0.95f, 0};

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