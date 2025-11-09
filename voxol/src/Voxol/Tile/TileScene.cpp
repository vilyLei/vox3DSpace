#include "TileScene.h"
#include "../Math/MathDef.h"
#include "../base/ScopeGuard.h"
#include <format>

namespace Voxol::Tile
{

void TileScene::initalize()
{
    if (unitIndexPool.capacity() > 0)
    {
        return;
    }
    //if (mFbo)
    //{
    //    return;
    //}
    //mFbo = Render::Draw::OglFbo::make();
    //mFbo->init(GL_ZERO);

    outlineUnit.drawUnit.vertex.toLine();
    Render::Gpu::buildBaseDrawUnit(outlineUnit.drawUnit);

    constexpr int UNITS_TOTAL = 256;
    unitIndexPool.init(UNITS_TOTAL);
    gridUnits.resize(UNITS_TOTAL);

    auto& srcUnit = gridUnits[0].drawUnit;
    Render::Gpu::buildTexDrawUnitWithTex(srcUnit, GL_ZERO, true);

    for (auto i = 1; i < gridUnits.size(); ++i)
    {
        auto& unit  = gridUnits[i].drawUnit;
        unit.vertex = srcUnit.vertex;
        unit.shader = srcUnit.shader;
    }
}

void TileScene::addDirtyBounds(const Math::Bounds& bounds, int phase)
{
    RC::Pos pos{};
    auto    gr = RC::xyRectToRCRect(bounds, currGridSize);
    auto    lv = viewGridLevel;
    for (auto r = gr.minR; r <= gr.maxR; r++)
    {
        for (auto c = gr.minC; c <= gr.maxC; c++)
        {
            pos.r                        = r;
            pos.c                        = c;
            pos.level                    = lv;
            dirtyUnitIndexMap[pos.value] = {pos, -1, phase};
        }
    }
    gridModifyDirty = true;
}

void TileScene::buildGridContent(Grid::Unit& unit, const Render::Draw::DrawContext& ctx)
{

    auto&  drawUnit = unit.drawUnit;
    auto&& pos      = drawUnit.objMat.getXY();

    auto scale = gridSize / unit.areaSize;

    Math::Mat33 vpMat = gridProjMat;

    Math::Mat33 viewM;
    viewM.setScaleXY(scale, scale);
    viewM.setXY(-pos.x * scale, -pos.y * scale);
    vpMat.append(viewM);

    Render::Draw::FBOCtxNode fboCtx;
    fboCtx.viewMat    = viewM;
    fboCtx.clearParam = clearParam;
    fboCtx.texUnits   = {{0, gridSize, gridSize, drawUnit.getTextureAt(0)}};

    std::string debugEnterInfo = std::format("Tile AAA RC({}, {})", static_cast<int>(unit.rc.r), static_cast<int>(unit.rc.c));
    std::string debugExitInfo  = std::format("Tile AAA RC({}, {})\n", static_cast<int>(unit.rc.r), static_cast<int>(unit.rc.c));

    auto&& nodeGuard      = ctx.makeFBOGraphNodeGuard(fboCtx, debugEnterInfo, debugExitInfo);

    auto&& xy = RC::rcToXY(unit.rc, currGridSize);
    auto&& vb = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
    ctx.drawCall(vb, vpMat);

    auto rttTex = nodeGuard.getRTTextureAt(0);
    printf("Tile >>> nodeGuard.getRTTextureAt(0): %d\n", rttTex);
    Render::Gpu::buildTexDrawUnitWithTex(drawUnit, rttTex, true);
}


bool TileScene::releaseGrid(const Grid::IndexNode& node)
{
    return releaseGrid(node.pos);
}
bool TileScene::releaseGrid(const RC::Pos& pos)
{

    if (!viewUnitIndexMap.contains(pos.value))
        return false;

    // remove an element from the viewUnitIndexMap
    auto&& node = viewUnitIndexMap[pos.value];
    unitIndexPool.release(node.index);
    auto& unit = gridUnits[node.index].drawUnit;
    printf("release grid node(r=%zu, c=%zu, level=%zu)\n", node.pos.r, node.pos.c, node.pos.level);
    texPool.release(unit.getTextureAt(0));
    return true;
}

void TileScene::testFreeViewGrids()
{
    for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end();)
    {
        auto& node = it->second;
        if (currRCRect.contains(node.pos) && node.pos.level == viewGridLevel)
        {
            ++it;
        }
        else
        {
            printf("remove an element from the viewUnitIndexMap ...\n");
            releaseGrid(node);
            it = viewUnitIndexMap.erase(it);
        }
    }
}

bool TileScene::updateGrid(const RC::Pos& pos, const Render::Draw::DrawContext& ctx, int phase)
{
    auto&& node = viewUnitIndexMap[pos.value];
    auto&& xy   = RC::rcToXY(pos, currGridSize);
    auto&& vb   = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
    auto&  grid = gridUnits[node.index];
    if (ctx.drawQueryCall(vb, phase))
    {
        //printf("TileScene::updateGrid() build content node(r=%d,c=%d) A, phase: %d\n", node.pos.r, node.pos.c, phase);
        grid.setRCAndAreaSize(pos, currGridSize);
        buildGridContent(grid, ctx);
        if (phase == 0)
        {
            emptyUnitIndexMap[node.pos.value] = {node.pos, false, phase};
        }
    }
    else
    {
        unitIndexPool.release(node.index);
        auto& unit = grid.drawUnit;
        //printf("TileScene::updateGrid() release node(r=%d, c=%d, level=%d) B, phase: %d\n", node.pos.r, node.pos.c, node.pos.level, phase);
        texPool.release(unit.getTextureAt(0));
        viewUnitIndexMap.erase(pos.value);
    }
    return true;
}

bool TileScene::createGrid(const RC::Pos& pos, const Render::Draw::DrawContext& ctx)
{
    auto&& xy = RC::rcToXY(pos, currGridSize);
    auto&& vb = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);
    if (!ctx.drawQueryCall(vb, 2))
    {
        return false;
    }

    auto k = unitIndexPool.acquire();
    if (k < 0)
    {
        return false;
    }
    viewUnitIndexMap[pos.value] = {pos, k};
    auto& grid                  = gridUnits[k];
    grid.setRCAndAreaSize(pos, currGridSize);
    grid.drawUnit.setTextureAt(texPool.acquire(), 0);
    buildGridContent(grid, ctx);
    return true;
}

void TileScene::updateDirtyGrid(const Render::Draw::DrawContext& ctx)
{

    if (dirtyUnitIndexMap.empty())
        return;

    for (auto&& it = dirtyUnitIndexMap.begin(); it != dirtyUnitIndexMap.end(); ++it)
    {
        auto& node = it->second;
        //printf("TileScene::updateDirtyGrid() node(r=%d, c=%d) ...\n", node.pos.r, node.pos.c);
        //if (!currRCRect.contains(node.pos)) {
        //    continue;
        //}

        //printf("TileScene::updateDirtyGrid() node(r=%d,c=%d,phase=%d) A\n", node.pos.r, node.pos.c, node.phase);
        if (viewUnitIndexMap.contains(node.pos.value))
        {
            updateGrid(node.pos, ctx, node.phase);
        }
        else
        {
            createGrid(node.pos, ctx);
        }
    }
    dirtyUnitIndexMap.clear();
}

void TileScene::updateEmptyGrid(const Render::Draw::DrawContext& ctx)
{
    if (emptyUnitIndexMap.empty())
        return;

    for (auto&& it = emptyUnitIndexMap.begin(); it != emptyUnitIndexMap.end(); ++it)
    {
        auto& node = it->second;
        if (node.index < 0)
            continue;

        //printf("TileScene::updateEmptyGrid() node(r=%d,c=%d,phase=%d) A\n", node.pos.r, node.pos.c, node.phase);
        auto&& xy = RC::rcToXY(node.pos, currGridSize);
        auto&& vb = Math::VxRect::makeXYWH(xy.x, xy.y, currGridSize, currGridSize);

        auto& grid = gridUnits[node.index];
        if (!ctx.drawQueryCall(vb, 0))
        {
            unitIndexPool.release(node.index);
            auto& unit = grid.drawUnit;

            printf("TileScene::updateEmptyGrid() release node(r=%zu, c=%zu, level=%zu) B, phase: %d\n", node.pos.r, node.pos.c, node.pos.level, 0);

            texPool.release(unit.getTextureAt(0));
            viewUnitIndexMap.erase(node.pos.value);
        }
        //printf("TileScene::updateEmptyGrid() node(r=%zu,c=%zu,phase=%d) B\n", node.pos.r, node.pos.c, node.phase);
    }
    emptyUnitIndexMap.clear();
}
void TileScene::run(const Render::Draw::DrawContext& ctx)
{
    auto&       drawParam = ctx.drawParam;
    Math::Mat33 vpM       = drawParam.projMat;
    vpM.append(drawParam.viewMat);

    //ctx.clearParam.apply();
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

    currGridSize = gridSize * lvScale;

    auto gr      = RC::xyRectToRCRect(drawParam.viewWBounds, currGridSize);
    auto grDirty = currRCRect.isNotEqual(gr);
    currRCRect   = gr;

    auto createFlag = ctx.dirty && viewGridLevel != lv;
    auto adjustFlag = ctx.dirty && viewGridLevel == lv;

    static float preZoom      = ctx.zoom;
    auto         toBiggerFlag = viewGridLevel < lv;
    preZoom                   = ctx.zoom;

    if (viewGridLevel != lv)
    {
        printf("XXXXX Curr lv: %d, toBiggerFlag: %s\n", lv, toBiggerFlag ? "true" : "false");
    }

    updateEmptyGrid(ctx);

    if (createFlag || adjustFlag || toBiggerFlag)
    {

        testFreeViewGrids();

        auto tot = 0;
        for (auto r = gr.minR; r <= gr.maxR; r++)
        {
            for (auto c = gr.minC; c <= gr.maxC; c++)
            {
                RC::Pos pos{r, c, lv};
                if (viewUnitIndexMap.contains(pos.value))
                {
                    if (toBiggerFlag)
                    {
                        updateGrid(pos, ctx, 2);
                    }
                    continue;
                }
                auto flag = createGrid(pos, ctx);
            }
        }
        //printf("append tot: %d\n", tot);
    }
    viewGridLevel = lv;

    updateDirtyGrid(ctx);
    //ctx.clearParam.apply();

    for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end(); it++)
    {
        auto& node = it->second;
        auto& unit = gridUnits[node.index].drawUnit;
        unit.mvp   = vpM;
        unit.draw();
    }

    texPool.dispose();

    //if (ctx.dirty)
    //{
    //    printf("TileScene::run(), tile grids total: %zu, lv:%d\n", viewUnitIndexMap.size(), lv);
    //}
    //return;
    //*
    //ctx.drawCall({}, vpM);


    outlineUnit.drawUnit.vertex.lineWidth = 3.0f;
    outlineUnit.drawUnit.setColor(0xff005555);
    for (auto&& it = viewUnitIndexMap.begin(); it != viewUnitIndexMap.end(); it++)
    {
        auto& node = it->second;
        auto& unit = gridUnits[node.index].drawUnit;

        outlineUnit.drawUnit.objMat = unit.objMat;
        outlineUnit.drawUnit.mvp    = vpM;
        outlineUnit.drawUnit.draw();
    }

    outlineUnit.drawUnit.vertex.lineWidth = 1.0f;
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