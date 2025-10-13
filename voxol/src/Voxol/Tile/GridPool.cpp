
#include "GridPool.h"

namespace Voxol::Tile
{
namespace Grid
{
void UnitIndexPool::init(size_t pool_size)
{
    for (int i = 0; i < pool_size; ++i)
    {
        free_indices.insert(i);
    }
}
int32_t UnitIndexPool::acquire()
{
    if (free_indices.empty())
    {
        return -1;
    }

    auto index = *free_indices.begin();
    if (busy_indices.contains(index))
    {
        printf("UnitIndexPool error acquire call ...\n");
    }
    free_indices.erase(free_indices.begin());
    busy_indices.insert(index);

    return index;
}

void UnitIndexPool::release(int32_t index)
{
    if (free_indices.contains(index) || !busy_indices.contains(index))
    {
        printf("UnitIndexPool error release call ...\n");
        return;
    }
    free_indices.insert(index);
    busy_indices.erase(index);
}
void UnitIndexPool::reset()
{
    if (free_indices.empty())
        return;

    for (auto k : busy_indices)
    {
        free_indices.insert(k);
    }
    busy_indices.clear();
}


GLuint UnitTexPool::acquire()
{
    if (free_textures.empty())
    {
        GLuint tex;
        glGenTextures(1, &tex);
        busy_textures.insert(tex);
        return tex;
    }

    auto tex = *free_textures.begin();
    if (busy_textures.contains(tex))
    {
        printf("UnitTexPool error acquire call ...\n");
    }
    free_textures.erase(free_textures.begin());
    busy_textures.insert(tex);

    return tex;
}

void UnitTexPool::release(GLuint tex)
{
    if (tex <= GL_ZERO)
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    if (free_textures.contains(tex))
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    if (!busy_textures.contains(tex))
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    free_textures.insert(tex);
    busy_textures.erase(tex);
}

void UnitTexPool::reset()
{
    if (busy_textures.empty())
        return;

    for (auto k : busy_textures)
    {
        free_textures.insert(k);
    }
    busy_textures.clear();
}

/// deffered destory some gpu textures per rendering frame
void UnitTexPool::dispose()
{
    constexpr int DISPOSE_NUM = 4;
    auto          tot         = free_textures.size();
    if (tot <= DISPOSE_NUM)
        return;
    std::vector<GLuint> ts(DISPOSE_NUM);
    auto                i = 0;
    for (auto tex : free_textures)
    {
        ts[i] = tex;
        i++;
        if (i >= 4)
            break;
    }
    for (auto tex : ts)
    {
        free_textures.erase(tex);
    }
    glDeleteTextures(DISPOSE_NUM, ts.data());
    printf("UnitTexPool::dispose() call tot: %zu, free_textures.size(): %zu\n", tot, free_textures.size());
}
}
}