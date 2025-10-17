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
        auto& unit  = gridUnits[i].drawUnit;
        unit.vertex = srcUnit.vertex;
        unit.shader = srcUnit.shader;
    }
}

void TileScene::addDirtyBounds(const Math::Bounds& bounds)
{
    auto gr = RC::xyRectToRCRect(bounds, currGridSize);
    auto lv = viewGridLevel;
    for (auto r = gr.minR; r <= gr.maxR; r++)
    {
        for (auto c = gr.minC; c <= gr.maxC; c++)
        {
            RC::Pos pos{r, c, lv};
            if (viewUnitIndexMap.contains(pos.value))
            {
                auto&& node = viewUnitIndexMap[pos.value];
                node.dirty  = true;
                continue;
            }
            dirtyUnitIndexMap[pos.value] = {pos, -1};
        }
    }
    gridModifyDirty = true;
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

    auto& drawParam = ctx.drawParam;
    auto& vp      = ctx.clearParam.viewport;

    mFbo.bindFBO();
    mFbo.bindTextureAt(drawUnit.getTextureAt(0), 0, gridSize, gridSize);
    mFbo.renderBegin(clearParam);

    ctx.drawCall(drawParam.viewWBounds, vpM);
    mFbo.unbindFBO(ctx.clearParam, true);

    Test::Gpu::buildTexDrawUnitWithTex(drawUnit, mFbo.getTextureAt(0), true);
}
void TileScene::run(const Render::Draw::DrawContext& ctx)
{
    auto&       drawParam = ctx.drawParam;
    Math::Mat33 vpM    = drawParam.projMat;
    vpM.append(drawParam.viewMat);

    // 暂时这样写，以便测试dragging
    //ctx.drawCall(drawParam.viewWBounds, vpM);
    //return;


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
    clearParam.viewport   = {0, 0, gridSize, gridSize};
    clearParam.clearColor = {0.95f, 0.95f, 0.95f, 0};

    // auto currGridSize = 256.0f;

    currGridSize = gridSize * lvScale;

    auto gr      = RC::xyRectToRCRect(drawParam.viewWBounds, currGridSize);
    auto grDirty = currRCRect.isNotEqual(gr);

    auto createFlag = ctx.dirty && viewGridLevel != lv;
    auto adjustFlag   = ctx.dirty && viewGridLevel == lv;

    static float preZoom      = ctx.zoom;
    //auto toBiggerFlag = preZoom < ctx.zoom || viewGridLevel < lv;
    auto toBiggerFlag = viewGridLevel < lv;
    preZoom = ctx.zoom;


    if (createFlag || adjustFlag || gridModifyDirty)
    {
        gridModifyDirty = false;
        for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end();)
        {
            auto& node = it->second;
            if (gr.contains(node.pos))
            {
                ++it;
            }
            else
            {
                // remove an element
                unitIndexPool.release(node.index);
                auto& unit = gridUnits[node.index].drawUnit;
                printf("erase a grid node(r=%d, c=%d, level=%d).\n", node.pos.r, node.pos.c, node.pos.level);
                texPool.release(unit.getTextureAt(0));
                it = viewUnitIndexMap.erase(it);
            }
        }

        auto tot = 0;
        for (auto r = gr.minR; r <= gr.maxR; r++)
        {
            for (auto c = gr.minC; c <= gr.maxC; c++)
            {
                RC::Pos pos{r, c, lv};
                if (viewUnitIndexMap.contains(pos.value))
                {
                    auto&& node = viewUnitIndexMap[pos.value];
                    if (toBiggerFlag)
                    {
                        auto&& xy = RC::rcToXY(pos, currGridSize);
                        auto&& vb   = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
                        auto&  grid = gridUnits[node.index];
                        if (!ctx.drawQuery(vb, vpM))
                        {

                            unitIndexPool.release(node.index);
                            auto& unit = grid.drawUnit;
                            printf("erase a grid node(r=%d, c=%d, level=%d) B.\n", node.pos.r, node.pos.c, node.pos.level);
                            texPool.release(unit.getTextureAt(0));
                            viewUnitIndexMap.erase(pos.value);
                            continue;
                        }
                        grid.setRCAndAreaSize(pos, currGridSize);
                        buildGrid(grid, ctx);
                    }

                    if (!node.dirty || node.index < 0)
                    {
                        continue;
                    }
                    else {
                        auto&& xy = RC::rcToXY(pos, currGridSize);
                        auto&& vb = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
                        if (!ctx.drawQuery(vb, vpM))
                        {
                            continue;
                        }
                        auto k = node.index;
                        node.dirty = true;

                        viewGridsTotal++;
                        tot++;

                        auto& grid = gridUnits[k];
                        grid.setRCAndAreaSize(pos, currGridSize);
                        grid.drawUnit.setTextureAt(texPool.acquire(), 0);
                        buildGrid(grid, ctx);
                    }
                }
                auto&& xy = RC::rcToXY(pos, currGridSize);
                auto&& vb = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
                if (!ctx.drawQuery(vb, vpM))
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
    viewGridLevel = lv;

    for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end(); it++)
    {
        auto& node = it->second;
        auto& unit = gridUnits[node.index].drawUnit;
        unit.mvp   = vpM;
        unit.draw();
    }

    texPool.dispose();

    if (ctx.dirty)
    {
        printf("TileScene::run(), tile grids total: %zu\n", viewUnitIndexMap.size());
    }
    return;
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
} // namespace Voxol::Tile