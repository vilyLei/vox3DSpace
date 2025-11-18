#ifndef VOXOL_BASE_HASH_UTILS_H
#define VOXOL_BASE_HASH_UTILS_H

#include "FNVHash.h"

namespace Voxol::Base
{
namespace Hash
{

// std::hash specialization
template <typename T>
void hashCombine(size_t& seed, const T& value)
{
    FNVHasher<64> hasher(seed);
    hasher.update(value);
    seed = hasher.finalize();
}

// range hash
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

// ordered container hash(order-independent)
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