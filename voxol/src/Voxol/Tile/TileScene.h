#ifndef VOXOL_TILE_SCENE_H
#define VOXOL_TILE_SCENE_H

#include "DrawCtx.h"
#include "GridPool.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/DrawCtx.h"
#include "../Test/OglResUtils.h"
#include "../Render/OglFbo.h"
#include <vector>


namespace Voxol::Tile
{
using namespace Voxol::Math;


class TileScene
{
public:
    TileScene()  = default;
    ~TileScene() = default;

public:
    void initalize();
    void buildGrid(Grid::Unit& unit, const Render::Draw::DrawContext& ctx);
    void run(const Render::Draw::DrawContext& ctx);

private:
    Render::OglFbo mFbo{};

    int gridSize = 256;

    std::vector<Grid::Unit> gridUnits{};
    Grid::Unit              outlineUnit{};

    Math::Mat33               gridProjMat{};
    Render::Draw::ClearParams clearParam{};
    int                       viewGridsTotal = 0;
    int                       viewGridLevel = 0;
    /// <summary>
    /// all units in the view scope 
    /// </summary>
    std::unordered_map<int64_t, Grid::IndexNode> viewUnitIndexMap{};

    Grid::UnitTexPool                            texPool{};
    Grid::UnitIndexPool                          unitIndexPool{};
    RC::Rect                                     currRCRect{};
};

} // namespace Voxol::Tile
#endif