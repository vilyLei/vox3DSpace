#ifndef VOXOL_BASE_HASH_UTILS_H
#define VOXOL_BASE_HASH_UTILS_H

#include "FNVHash.h"

namespace Voxol::Base
{
namespace Hash
{

// 组合哈希值（用于std::hash特化）
template <typename T>
void hashCombine(size_t& seed, const T& value)
{
    FNVHasher<64> hasher(seed);
    hasher.update(value);
    seed = hasher.finalize();
}

// 范围哈希
template <typename Iterator>
auto hashRange(Iterator begin, Iterator end)
{
    FNVHasher<64> hasher;
    for (auto it = begin; it != end; ++it)
    {
        hasher.update(*it);
    }
    return hasher.finalize();
}

// 对有序容器的哈希（不考虑顺序）
template <typename Container>
auto hashOrderedContainer(const Container& container)
{
    auto sorted = container;
    std::sort(sorted.begin(), sorted.end());
    return hashRange(sorted.begin(), sorted.end());
}

} // namespace Hash
} // namespace Voxol::Base
#endif