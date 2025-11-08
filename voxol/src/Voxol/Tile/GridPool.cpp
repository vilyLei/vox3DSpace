
#include "GridPool.h"

namespace Voxol::Tile
{
namespace Grid
{
void UnitIndexPool::init(size_t poolSize)
{
    freeIndices.resize(poolSize);
    for (int i = 0; i < poolSize; ++i)
    {
        freeIndices[i] = i;
    }
    used.assign(poolSize, 0);
}

int32_t UnitIndexPool::acquire()
{

    if (freeIndices.empty())
    {
        return -1;
    }
    auto index = freeIndices.back();
    freeIndices.pop_back();
    used[index] = 1;

    return index;
}

void UnitIndexPool::release(int32_t index)
{
    if (index < 0 || index >= static_cast<int32_t>(used.size()))
    {
        printf("UnitIndexPool::release() release invalid index.\n");
        return;
    }
    if (!used[index])
    {
        printf("UnitIndexPool::release() double release index.\n");
        return;
    }
    used[index] = 0;
    freeIndices.push_back(index);
}
void UnitIndexPool::reset()
{
    if (freeIndices.empty())
        return;

    for (auto k : freeIndices)
    {
        used[k] = 0;
    }
    freeIndices.clear();
    auto poolSize = static_cast<int32_t>(used.size());
    for (int i = 0; i < poolSize; ++i)
    {
        freeIndices.push_back(i);
    }
}

size_t UnitIndexPool::capacity() const
{
    return freeIndices.capacity();
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
} // namespace Grid
} // namespace Voxol::Tile