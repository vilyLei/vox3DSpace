#ifndef VOXOL_TILE_GRID_POOL_H
#define VOXOL_TILE_GRID_POOL_H

#include "GridDef.h"
#include <unordered_set>
#include <unordered_map>

namespace Voxol::Tile
{
namespace Grid
{
class UnitIndexPool
{
public:
    UnitIndexPool() = default;

public:
    void    init(size_t pool_size);
    int32_t acquire();
    void    release(int32_t index);
    void    reset();

private:
    std::unordered_set<int> busyIndices{};
    std::unordered_set<int> freeIndices{};
};

class UnitTexPool
{
public:
    UnitTexPool() = default;

public:
    GLuint acquire();
    void   release(GLuint tex);
    void   reset();
    /// deffered destory some gpu textures per rendering frame
    void dispose();

private:
    std::unordered_set<GLuint> busyTextures{};
    std::unordered_set<GLuint> freeTextures{};
};

}
}
#endif