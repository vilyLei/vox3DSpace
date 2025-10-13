
#include "GridPool.h"

namespace Voxol::Tile
{
namespace Grid
{
void UnitIndexPool::init(size_t pool_size)
{
    for (int i = 0; i < pool_size; ++i)
    {
        freeIndices.insert(i);
    }
}
int32_t UnitIndexPool::acquire()
{
    if (freeIndices.empty())
    {
        return -1;
    }

    auto index = *freeIndices.begin();
    if (busyIndices.contains(index))
    {
        printf("UnitIndexPool error acquire call ...\n");
    }
    freeIndices.erase(freeIndices.begin());
    busyIndices.insert(index);

    return index;
}

void UnitIndexPool::release(int32_t index)
{
    if (freeIndices.contains(index) || !busyIndices.contains(index))
    {
        printf("UnitIndexPool error release call ...\n");
        return;
    }
    freeIndices.insert(index);
    busyIndices.erase(index);
}
void UnitIndexPool::reset()
{
    if (freeIndices.empty())
        return;

    for (auto k : busyIndices)
    {
        freeIndices.insert(k);
    }
    busyIndices.clear();
}


GLuint UnitTexPool::acquire()
{
    if (freeTextures.empty())
    {
        GLuint tex;
        glGenTextures(1, &tex);
        busyTextures.insert(tex);
        return tex;
    }

    auto tex = *freeTextures.begin();
    if (busyTextures.contains(tex))
    {
        printf("UnitTexPool error acquire call ...\n");
    }
    freeTextures.erase(freeTextures.begin());
    busyTextures.insert(tex);

    return tex;
}

void UnitTexPool::release(GLuint tex)
{
    if (tex <= GL_ZERO)
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    if (freeTextures.contains(tex))
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    if (!busyTextures.contains(tex))
    {
        printf("UnitTexPool error release call ...\n");
        return;
    }
    freeTextures.insert(tex);
    busyTextures.erase(tex);
}

void UnitTexPool::reset()
{
    if (busyTextures.empty())
        return;

    for (auto k : busyTextures)
    {
        freeTextures.insert(k);
    }
    busyTextures.clear();
}

/// deffered destory some gpu textures per rendering frame
void UnitTexPool::dispose()
{
    constexpr int DISPOSE_NUM = 4;
    auto          tot         = freeTextures.size();
    if (tot <= DISPOSE_NUM)
        return;
    std::vector<GLuint> ts(DISPOSE_NUM);
    auto                i = 0;
    for (auto tex : freeTextures)
    {
        ts[i] = tex;
        i++;
        if (i >= 4)
            break;
    }
    for (auto tex : ts)
    {
        freeTextures.erase(tex);
    }
    glDeleteTextures(DISPOSE_NUM, ts.data());
    printf("UnitTexPool::dispose() call tot: %zu, freeTextures.size(): %zu\n", tot, freeTextures.size());
}
}
}