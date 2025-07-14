#ifndef VOXOL_SLAB_ARENA_H
#define VOXOL_SLAB_ARENA_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <vector>
#include "BaseDefine.h"


namespace Voxol::Base {

class SlabArenaBlock {
public:
  SlabArenaBlock(size_t size);

  ~SlabArenaBlock();

  void *allocate(size_t size, size_t alignment);

  void reset();

private:
  void *data_;
  size_t size_;
  size_t offset_;
};

class SlabArena {
public:
  void *allocate(size_t size, size_t alignment = alignof(std::max_align_t));
  void reset();
  template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* memory = allocate(sizeof(T), alignof(T));
        return std::construct_at(reinterpret_cast<T*>(memory), std::forward<Args>(args)...);
    }

private:
  std::vector<std::unique_ptr<SlabArenaBlock>> blocks;
};

// Slab allocator for fixed-size type T
template <typename T> class SlabPool {
public:
  explicit SlabPool(SlabArena &arena) : arena_(arena) {}

  T *allocate() {
    if (!freeList.empty()) {
      T *obj = freeList.back();
      freeList.pop_back();
      return obj;
    }
    void *mem = arena_.allocate(sizeof(T), alignof(T));
    return std::construct_at(reinterpret_cast<T *>(mem));
  }

  void deallocate(T *obj) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      obj->~T();
    }
    freeList.push_back(obj);
  }

  void reset() { freeList.clear(); }

private:
  SlabArena &arena_;
  std::vector<T *> freeList;
};

} // namespace Voxol::Base
#endif  // VOXOL_SLAB_ARENA_H