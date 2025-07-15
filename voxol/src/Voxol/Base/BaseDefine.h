#ifndef VOXOL_BASE_DEFINE_H
#define VOXOL_BASE_DEFINE_H

#include <cstdint>


constexpr size_t DEFAULT_SLAB_BLOCK_SIZE = 64 * 1024;

using VoxolEntity = uint32_t;
constexpr VoxolEntity VoxolEntity_None = 0;

#endif  // VOXOL_BASE_DEFINE_H