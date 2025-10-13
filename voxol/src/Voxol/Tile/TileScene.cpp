#include "TileScene.h"
#include "../Math/MathDef.h"

namespace Voxol::Tile
{
    
    void TileScene::initalize()
    {
        mFbo.init(GL_ZERO);

        outlineUnit.drawUnit.vertex.toLine();
        Test::Gpu::buildBaseDrawUnit(outlineUnit.drawUnit);

        constexpr int UNITS_TOTAL = 256;
        unitIndexPool.init(UNITS_TOTAL);
        gridUnits.resize(UNITS_TOTAL);

        auto& srcUnit = gridUnits[0].drawUnit;
        Test::Gpu::buildTexDrawUnitWithTex(srcUnit, GL_ZERO, true);

        for (auto i = 1; i < gridUnits.size(); ++i)
        {
            auto& unit = gridUnits[i].drawUnit;
            unit.vertex = srcUnit.vertex;
            unit.shader = srcUnit.shader;
        }
        
    }

    void TileScene::buildGrid(Grid::Unit& unit, const Render::Draw::DrawContext& ctx)
    {

        auto&  drawUnit = unit.drawUnit;
        auto&& pos      = drawUnit.objMat.getXY();

        auto scale = gridSize / unit.areaSize;

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


        // the default gridSize value is 256
        auto value = gridSize * ctx.zoom;
        auto lv    = Math::calcCeilOfTwoLevelToInt(value);

        auto dstSize = std::pow(2, lv);
        auto lvScale = float(gridSize) / dstSize;
        //if (ctx.dirty)
        //{
        //    printf("TileScene::run(), value: %f, lv: %d, dstSize: %f, lvScale: %f\n", value, lv, dstSize, lvScale);
        //}

        gridProjMat.ortho(gridSize, gridSize);
        clearParam.viewport = {0, 0, gridSize, gridSize};
        clearParam.clearColor = {0.95f, 0.95f, 0.95f, 0};

        auto currGridSize = 256.0f;

        currGridSize = gridSize * lvScale;


        outlineUnit.drawUnit.setColor(0x50550055);

        //auto k = 0;
        auto gr = RC::xyRectToRCRect(params.viewWBounds, currGridSize);
        auto grDirty = currRCRect.isNotEqual(gr);

        auto createFlag = ctx.dirty && viewGridLevel != lv;
        ///*
        if (createFlag)
        {
            for (auto& e : unitIndexMap)
            {
                auto  k    = e.second.index;
                auto& unit = gridUnits[k].drawUnit;
                texPool.release(unit.getTextureAt(0));
            }

            unitIndexMap.clear();
            unitIndexPool.reset();

            viewGridsTotal = 0;
            for (auto r = gr.minR; r <= gr.maxR; r++)
            {
                for (auto c = gr.minC; c <= gr.maxC; c++)
                {
                    //gridUnits[k].setRCAndAreaSize({r, c}, currGridSize);
                    //buildGrid(gridUnits[k], ctx);

                    RC::Pos pos = {r, c, lv};

                    auto  k    = unitIndexPool.acquire();
                    auto& grid = gridUnits[k];
                    grid.setRCAndAreaSize(pos, currGridSize);
                    grid.drawUnit.setTextureAt(texPool.acquire(), 0);
                    buildGrid(grid, ctx);

                    unitIndexMap[pos.value] = {pos.value, k};

                    viewGridsTotal++;
                }
            }
            printf("create tile grids tot: %d\n", viewGridsTotal);
            //gridsTotal = k;
        }

        for (auto& e : unitIndexMap)
        {
            auto  k    = e.second.index;
            auto& unit = gridUnits[k].drawUnit;
            unit.mvp   = vpM;
            unit.draw();
        }
        //for (auto i = 0; i < gridsTotal; i++)
        //{
        //    gridUnits[i].drawUnit.mvp = vpM;
        //    gridUnits[i].drawUnit.draw();
        //}

        //if (ctx.dirty)
        //{
        //    printf("TileScene::run(), tile grids total: %d\n", k);
        //}
        return;
        //*/
        //*
        //ctx.drawCall({}, vpM);
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
        //*/
    }
}