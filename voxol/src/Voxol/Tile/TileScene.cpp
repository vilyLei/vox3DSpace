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

        auto gr = RC::xyRectToRCRect(params.viewWBounds, currGridSize);
        auto grDirty = currRCRect.isNotEqual(gr);

        auto createFlag = ctx.dirty && viewGridLevel != lv;
        auto adjustFlag = ctx.dirty && viewGridLevel == lv;
        viewGridLevel = lv;
        ///*
        if (createFlag)
        {
            for (auto& e : viewUnitIndexMap)
            {
                auto  k    = e.second.index;
                auto& unit = gridUnits[k].drawUnit;
                texPool.release(unit.getTextureAt(0));
            }

            viewUnitIndexMap.clear();
            unitIndexPool.reset();

            viewGridsTotal = 0;
            for (auto r = gr.minR; r <= gr.maxR; r++)
            {
                for (auto c = gr.minC; c <= gr.maxC; c++)
                {
                    auto k = unitIndexPool.acquire();
                    if (k < 0) {
                        continue;
                    }

                    RC::Pos pos = {r, c, lv};

                    auto& grid = gridUnits[k];
                    grid.setRCAndAreaSize(pos, currGridSize);
                    grid.drawUnit.setTextureAt(texPool.acquire(), 0);
                    buildGrid(grid, ctx);

                    viewUnitIndexMap[pos.value] = {pos, k};

                    viewGridsTotal++;
                }
            }
            printf("create tile grids tot: %d\n", viewGridsTotal);
        }
        if (adjustFlag)
        {
            auto tot = 0;
            for (auto r = gr.minR; r <= gr.maxR; r++)
            {
                for (auto c = gr.minC; c <= gr.maxC; c++)
                {
                    RC::Pos pos = {r, c, lv};
                    if (viewUnitIndexMap.contains(pos.value))
                    {
                        continue;
                    }
                    auto k = unitIndexPool.acquire();
                    if (k < 0)
                    {
                        continue;
                    }
                    viewGridsTotal++;
                    tot++;

                    auto& grid = gridUnits[k];
                    grid.setRCAndAreaSize(pos, currGridSize);

                    grid.drawUnit.setTextureAt(texPool.acquire(), 0);

                    buildGrid(grid, ctx);
                    viewUnitIndexMap[pos.value] = {pos, k};
                }
            }
            printf("append tot: %d\n", tot);
        }

        for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end();)
        {
            auto& node = it->second;
            auto  k    = node.index;
            if (gr.contains(node.pos))
            {
                auto& unit = gridUnits[k].drawUnit;
                unit.mvp   = vpM;
                unit.draw();
                ++it;
            }
            else
            {
                // ×¼±¸ÒÆ³ý
                unitIndexPool.release(k);
                auto& unit = gridUnits[k].drawUnit;
                printf("erase a grid node(r=%d, c=%d, level=%d).\n", node.pos.r, node.pos.c, node.pos.level);
                texPool.release(unit.getTextureAt(0));
                it = viewUnitIndexMap.erase(it);
            }
        }

        texPool.dispose();

        if (ctx.dirty)
        {
            printf("TileScene::run(), tile grids total: %zu\n", viewUnitIndexMap.size());
        }
        return;
        //*/
        //*
        //ctx.drawCall({}, vpM);


        outlineUnit.drawUnit.setColor(0x50550055);
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