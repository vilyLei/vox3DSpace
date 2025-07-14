#include "SlabArena.h"
#include <stdio.h>

namespace Voxol::Base {
SlabArenaBlock::SlabArenaBlock(size_t size)
    : data_(::operator new(size, std::align_val_t(alignof(std::max_align_t)))),
      size_(size), offset_(0) {
        printf("SlabArenaBlock::SlabArenaBlock() ...\n");
      }

SlabArenaBlock::~SlabArenaBlock() {
  if (data_) {
    ::operator delete(data_, std::align_val_t(alignof(std::max_align_t)));
    data_ = nullptr;
        printf("SlabArenaBlock::~SlabArenaBlock() ...\n");
  }
}

void *SlabArenaBlock::allocate(size_t size, size_t alignment) {
  size_t current = reinterpret_cast<size_t>(data_) + offset_;
  size_t aligned = (current + alignment - 1) & ~(alignment - 1);
  size_t newOffset = aligned - reinterpret_cast<size_t>(data_) + size;

  if (newOffset > size_)
    return nullptr;

  offset_ = newOffset;
  return reinterpret_cast<void *>(aligned);
}

void SlabArenaBlock::reset() { offset_ = 0; }

void *SlabArena::allocate(size_t size,
                          size_t alignment) {
  if (blocks.empty() || blocks.back()->allocate(size, alignment) == nullptr) {
    blocks.emplace_back(std::make_unique<SlabArenaBlock>(
        std::max(DEFAULT_SLAB_BLOCK_SIZE, size)));
  }
  return blocks.back()->allocate(size, alignment);
}

void SlabArena::reset() {
  for (auto &block : blocks) {
    block->reset();
  }
}
} // namespace Voxol::Base