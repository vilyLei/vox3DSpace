#ifndef VOXOL_TASK_MPMC_QUEUE_H
#define VOXOL_TASK_MPMC_QUEUE_H

#include <cstdio>
#include <atomic>
#include <array>
#include <memory>
#include <cstdlib>
#include <optional>

#ifdef _WIN32
#    include <malloc.h>
#endif

#include <thread>
#include <iostream>

namespace Voxol::Task::Cocurrent
{

inline void* aligned_malloc(std::size_t size, std::size_t alignment)
{
#ifdef _WIN32
    void* ptr = _aligned_malloc(size, alignment);
    if (!ptr) throw std::bad_alloc();
    return ptr;
#else
    void* ptr = std::aligned_alloc(alignment, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
#endif
}

inline void aligned_free(void* ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size  = 64;
#endif

template <typename T>
class Slot
{
public:
    alignas(hardware_constructive_interference_size) std::atomic<size_t> sequence{0};
    alignas(hardware_constructive_interference_size) void* storage = nullptr;

    Slot(const Slot&)            = delete;
    Slot& operator=(const Slot&) = delete;

    Slot(Slot&& other) noexcept
    {

        storage = other->storage;
        sequence.store(other.sequence.load(std::memory_order_relaxed));

        other->storage = nullptr;
    }
    Slot& operator=(Slot&& other) noexcept
    {

        if (this == &other) return *this;

        storage = other->storage;
        sequence.store(other.sequence.load(std::memory_order_relaxed));
        other->storage = nullptr;
        return *this;
    }

    Slot()
    {
        buildStorage();
    }

    ~Slot()
    {
        if (!storage)
            return;

        std::destroy_at(static_cast<T*>(storage));
        aligned_free(storage);
        storage = nullptr;
//#ifdef _WIN32
//        _aligned_free(storage);
//#else
//        std::free(storage);
//#endif
    }
    void buildStorage()
    {

        if (storage)
            return;
        storage = aligned_malloc(alignof(T), sizeof(T));
        //#ifdef _WIN32
//        storage = _aligned_malloc(sizeof(T), alignof(T));
//#else
//        storage = static_cast<void*>(std::aligned_alloc(alignof(T), storageSize));
//#endif
//        if (!storage) throw std::bad_alloc();
    }
    T* get()
    {
        return static_cast<T*>(storage);
    }
    T load()
    {
        auto ptr = get();
        storage  = nullptr;
        return std::move(*ptr);
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        buildStorage();
        std::construct_at(static_cast<T*>(storage), std::forward<Args>(args)...);
    }

    void destroy()
    {
        std::destroy_at(static_cast<T*>(storage));
    }
};

template <typename T, size_t Capacity = 1024>
class MPMCQueue
{
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

    static constexpr size_t CacheLineSize = hardware_destructive_interference_size;
    static constexpr size_t IndexMask     = Capacity - 1;

    alignas(CacheLineSize) Slot<T>* slots[Capacity];
    alignas(CacheLineSize) std::atomic<size_t> head{0};
    alignas(CacheLineSize) std::atomic<size_t> tail{0};

public:
    MPMCQueue()
    {
        for (size_t i = 0; i < Capacity; ++i)
        {
            slots[i] = new Slot<T>();
            slots[i]->sequence.store(i, std::memory_order_relaxed);
        }
    }

    ~MPMCQueue()
    {
        for (size_t i = 0; i < Capacity; ++i)
        {
            Slot<T>* slot = slots[i];
            delete slots[i];
            slots[i] = nullptr;
        }
    }

    bool try_enqueue(const T& value)
    {
        return emplace([&](Slot<T>& slot) { slot.construct(value); });
    }

    bool try_enqueue(T&& value)
    {
        return emplace([&](Slot<T>& slot) { slot.construct(std::move(value)); });
    }

    std::optional<T> try_dequeue()
    {
        size_t pos = head.load(std::memory_order_relaxed);
        while (true)
        {
            Slot<T>* slot = slots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            if (diff == 0)
            {
                if (head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                {
                    T&& value = slot->load();
                    slot->sequence.store(pos + Capacity, std::memory_order_release);
                    return value;
                }
            }
            else if (diff < 0)
            {
                return std::nullopt;
            }
            else
            {
                pos = head.load(std::memory_order_relaxed);
            }
        }
    }

    size_t try_batch_enqueue(const T* items, size_t count)
    {
        size_t tailValue = tail.load(std::memory_order_relaxed);
        size_t written   = 0;
        for (; written < count; ++written)
        {
            size_t   pos  = tailValue + written;
            Slot<T>* slot = slots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            if (diff != 0)
                break;
        }

        if (written == 0) return 0;

        if (!tail.compare_exchange_strong(tailValue, tailValue + written, std::memory_order_relaxed))
            return 0;

        for (size_t i = 0; i < written; ++i)
        {
            size_t   pos  = tailValue + i;
            Slot<T>* slot = slots[pos & IndexMask];
            slot->construct(items[i]);
            slot->sequence.store(pos + 1, std::memory_order_release);
        }

        return written;
    }

    size_t try_batch_dequeue(T* out_items, size_t max_count)
    {
        size_t headValue = head.load(std::memory_order_relaxed);
        size_t read      = 0;
        for (; read < max_count; ++read)
        {
            size_t   pos  = headValue + read;
            Slot<T>* slot = slots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            if (diff != 0)
                break;
        }

        if (read == 0) return 0;

        if (!head.compare_exchange_strong(headValue, headValue + read, std::memory_order_relaxed))
            return 0;

        for (size_t i = 0; i < read; ++i)
        {
            size_t   pos  = headValue + i;
            Slot<T>* slot = slots[pos & IndexMask];
            out_items[i]  = slot->load();
            slot->sequence.store(pos + Capacity, std::memory_order_release);
        }

        return read;
    }
    bool try_dequeue_pair(T& a, T& b)
    {
        auto pa = try_dequeue();
        if (!pa.has_value()) return false;

        auto pb = try_dequeue();
        if (!pb.has_value())
        {
            size_t spin = 0;
            auto&& ma   = pa.value();
            // an experiment test
            while (!try_enqueue(ma))
            {
                if (++spin > 1000)
                {
                    std::cerr << "Rollback failed: dropping one matrix.\n";
                    break;
                }
                std::this_thread::yield();
            }
            return false;
        }
        a = pa.value();
        b = pb.value();
        return true;
    }

private:
    template <typename Fn>
    bool emplace(Fn&& store_fn)
    {
        size_t tailValue = tail.load(std::memory_order_relaxed);
        while (true)
        {
            Slot<T>* slot = slots[tailValue & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail);
            if (diff == 0)
            {
                if (tail.compare_exchange_weak(tailValue, tailValue + 1, std::memory_order_relaxed))
                {
                    store_fn(*slot);
                    slot->sequence.store(tailValue + 1, std::memory_order_release);
                    return true;
                }
            }
            else if (diff < 0)
            {
                // The queue is already full
                return false;
            }
            else
            {
                tailValue = tail.load(std::memory_order_relaxed);
            }
        }
    }
};

/*
#include <cstdlib>
#include <memory>
#include <atomic>
#include <array>
#include <optional>

#ifdef _WIN32
#    include <malloc.h> // for _aligned_malloc/_aligned_free
#endif
namespace Voxol::Task::Cocurrent
{
inline void* aligned_malloc(std::size_t size, std::size_t alignment)
{
#ifdef _WIN32
    void* ptr = _aligned_malloc(size, alignment);
    if (!ptr) throw std::bad_alloc();
    return ptr;
#else
    void* ptr = std::aligned_alloc(alignment, size);
    if (!ptr) throw std::bad_alloc();
    return ptr;
#endif
}

inline void aligned_free(void* ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}
#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size  = 64;
#endif

template <typename T>
class Slot
{
public:
    alignas(hardware_constructive_interference_size) std::atomic<size_t> sequence{0};
    alignas(hardware_constructive_interference_size) void* storage = nullptr;

    Slot()
    {
        storage = aligned_malloc(alignof(T), sizeof(T));
        if (!storage) throw std::bad_alloc();
    }

    ~Slot()
    {
        std::destroy_at(static_cast<T*>(storage));
        aligned_free(storage);
    }

    T* get()
    {
        return static_cast<T*>(storage);
    }
    T load()
    {
        return std::move( *get() );
    }

    template <typename... Args>
    void construct(Args&&... args)
    {
        std::construct_at(static_cast<T*>(storage), std::forward<Args>(args)...);
    }

    void destroy()
    {
        if(!storage)
            std::destroy_at(static_cast<T*>(storage));
    }
};

template <typename T, size_t Capacity = 1024>
class MPMCQueue
{
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

    static constexpr size_t CacheLineSize = 64;
    static constexpr size_t IndexMask     = Capacity - 1;

    // struct Slot {
    //     alignas(CacheLineSize) std::atomic<size_t> sequence;
    //     alignas(CacheLineSize) std::aligned_storage_t<sizeof(T), alignof(T)> storage;
    //     T* data_ptr() noexcept {
    //         return reinterpret_cast<T*>(&storage);
    //     }
    //     void store(const T& value) {
    //         new (&storage) T(value);
    //     }
    //     void store(T&& value) {
    //         new (&storage) T(std::move(value));
    //     }
    //     T load() {
    //         return std::move(*data_ptr());
    //     }
    //     void destroy() noexcept {
    //         data_ptr()->~T();
    //     }
    // };

    alignas(CacheLineSize) std::array<Slot<T>*, Capacity> mSlots;
    alignas(CacheLineSize) std::atomic<size_t> mHead{0};
    alignas(CacheLineSize) std::atomic<size_t> mTail{0};

public:
    MPMCQueue()
    {
        for (size_t i = 0; i < Capacity; ++i)
        {
            mSlots[i]->sequence.store(i, std::memory_order_relaxed);
        }
    }

    ~MPMCQueue()
    {
        // clear
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < Capacity; ++i)
            {
                Slot<T>* slot = mSlots[i];
                size_t   seq  = slot->sequence.load(std::memory_order_relaxed);
                size_t   pos  = i;
                if (seq == pos + 1)
                {
                    slot->destroy();
                }
            }
        }
    }

    bool try_enqueue(const T& value)
    {
        return emplace([&](Slot<T>& slot) { slot.store(value); });
    }

    bool try_enqueue(T&& value)
    {
        return emplace([&](Slot<T>& slot) { slot.store(std::move(value)); });
    }

    std::optional<T> try_dequeue()
    {
        size_t pos = mHead.load(std::memory_order_relaxed);
        while (true)
        {
            Slot&    slot = mSlots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            if (diff == 0)
            {
                if (mHead.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                {
                    T value = slot->load();
                    slot->destroy();
                    slot->sequence.store(pos + Capacity, std::memory_order_release);
                    return value;
                }
            }
            else if (diff < 0)
            {
                return std::nullopt; // empty
            }
            else
            {
                pos = mHead.load(std::memory_order_relaxed);
            }
        }
    }

    size_t try_batch_enqueue(const T* items, size_t count)
    {
        size_t tail    = mTail.load(std::memory_order_relaxed);
        size_t written = 0;
        for (; written < count; ++written)
        {
            size_t   pos  = tail + written;
            Slot<T>*    slot = mSlots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
            if (diff != 0)
                break;
        }

        if (written == 0) return 0;

        if (!mTail.compare_exchange_strong(tail, tail + written, std::memory_order_relaxed))
            return 0;

        for (size_t i = 0; i < written; ++i)
        {
            size_t pos  = tail + i;
            Slot<T>*  slot = mSlots[pos & IndexMask];
            slot->store(items[i]);
            slot->sequence.store(pos + 1, std::memory_order_release);
        }

        return written;
    }

    size_t try_batch_dequeue(T* out_items, size_t max_count)
    {
        size_t head = mHead.load(std::memory_order_relaxed);
        size_t read = 0;
        for (; read < max_count; ++read)
        {
            size_t   pos  = head + read;
            Slot<T>*    slot = mSlots[pos & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
            if (diff != 0)
                break;
        }

        if (read == 0) return 0;

        if (!mHead.compare_exchange_strong(head, head + read, std::memory_order_relaxed))
            return 0;

        for (size_t i = 0; i < read; ++i)
        {
            size_t pos   = head + i;
            Slot<T>*  slot  = mSlots[pos & IndexMask];
            out_items[i] = slot->load();
            slot->destroy();
            slot->sequence.store(pos + Capacity, std::memory_order_release);
        }

        return read;
    }

private:
    template <typename Fn>
    bool emplace(Fn&& store_fn)
    {
        size_t tail = mTail.load(std::memory_order_relaxed);
        while (true)
        {
            Slot<T>*    slot = mSlots[tail & IndexMask];
            size_t   seq  = slot->sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail);
            if (diff == 0)
            {
                if (mTail.compare_exchange_weak(tail, tail + 1, std::memory_order_relaxed))
                {
                    store_fn(*slot);
                    slot->sequence.store(tail + 1, std::memory_order_release);
                    return true;
                }
            }
            else if (diff < 0)
            {
                return false; // full
            }
            else
            {
                tail = mTail.load(std::memory_order_relaxed);
            }
        }
    }
};
} // namespace Voxol::Task::Cocurrent
//*/
void testMain();
} // namespace Voxol::Task::Cocurrent
#endif  // VOXOL_TASK_MPMC_QUEUE_H