#ifndef VOXOL_TILE_GRID_POOL_H
#define VOXOL_TILE_GRID_POOL_H

#include "GridDef.h"
#include <vector>
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
    void    init(size_t poolSize);
    int32_t acquire();
    void    release(int32_t index);
    void    reset();
    size_t  capacity() const;

private:
    std::vector<int> used{};
    std::vector<int> freeIndices{};
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

} // namespace Grid
} // namespace Voxol::Tile
#endif