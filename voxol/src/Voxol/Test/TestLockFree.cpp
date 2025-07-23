#include "TestLockFree.h"
#include <cstdio>
#include <atomic>
#include <thread>
#include <mutex>

namespace Voxol::Test::LockFree
{
namespace Demo1
{
class SeqCstCounter
{
    std::atomic<int> count{0};

public:
    void increment()
    {
        count.fetch_add(1); // 默认 memory_order_seq_cst
    }

    int get() const
    {
        return count.load(); // 保证全序一致
    }
};

template <typename T, size_t N>
class LockFreeRingQueue {
public:
    LockFreeRingQueue() : head_(0), tail_(0) {}

    bool enqueue(const T& item) {
        size_t tail = tail_.load(std::memory_order_relaxed);
        size_t next_tail = (tail + 1) % N;

        // 加 acquire，确保读取有效数据
        if (next_tail == head_.load(std::memory_order_acquire))
            return false; // full

        buffer_[tail] = item;

        // release 确保 buffer_ 写入先于 tail_ 更新
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    bool dequeue(T& item) {
        size_t head = head_.load(std::memory_order_relaxed);
        // 加 acquire，确保 buffer_ 读取是有效的
        if (head == tail_.load(std::memory_order_acquire))
            return false; // empty

        item = buffer_[head];

        // release 确保读出后才移动 head_
        head_.store((head + 1) % N, std::memory_order_release);
        return true;
    }

private:
    T buffer_[N];
    std::atomic<size_t> head_;
    std::atomic<size_t> tail_;
};
class WaitForZero
{
public:
    void inc()
    {
        counter.fetch_add(1, std::memory_order_relaxed);
    }

    void dec()
    {
        if (counter.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.notify_all();
        }
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return counter.load(std::memory_order_acquire) == 0; });
    }

private:
    std::atomic<int>        counter{0};
    std::mutex              mtx;
    std::condition_variable cv;
};

template <typename T, size_t Size>
class MPMCQueue
{
public:
    MPMCQueue()
    {
        for (size_t i = 0; i < Size; ++i)
        {
            buffer[i].seq.store(i, std::memory_order_relaxed);
        }
        head.store(0, std::memory_order_relaxed);
        tail.store(0, std::memory_order_relaxed);
    }

    bool enqueue(const T& item);
    bool dequeue(T& item);

private:
    struct Slot
    {
        std::atomic<size_t> seq;
        T                   data;
    };

    alignas(64) Slot buffer[Size];
    alignas(64) std::atomic<size_t> head;
    alignas(64) std::atomic<size_t> tail;
};
template <typename T, size_t Size>
bool MPMCQueue<T, Size>::enqueue(const T& item)
{
    size_t pos = tail.load(std::memory_order_relaxed);

    for (;;)
    {
        Slot&    slot = buffer[pos % Size];
        size_t   seq  = slot.seq.load(std::memory_order_acquire);
        intptr_t dif  = (intptr_t)seq - (intptr_t)pos;

        if (dif == 0)
        {
            // 尝试占有这个位置
            if (tail.compare_exchange_weak(pos, pos + 1,
                                           std::memory_order_relaxed))
            {
                // 拿到了位置，写数据
                slot.data = item;
                // 标记该槽位现在可读
                slot.seq.store(pos + 1, std::memory_order_release);
                return true;
            }
        }
        else if (dif < 0)
        {
            // 缓冲区满
            return false;
        }
        else
        {
            // 另一个线程更新了 tail，重试
            pos = tail.load(std::memory_order_relaxed);
        }
    }
}
template <typename T, size_t Size>
bool MPMCQueue<T, Size>::dequeue(T& item)
{
    size_t pos = head.load(std::memory_order_relaxed);

    for (;;)
    {
        Slot&    slot = buffer[pos % Size];
        size_t   seq  = slot.seq.load(std::memory_order_acquire);
        intptr_t dif  = (intptr_t)seq - (intptr_t)(pos + 1);

        if (dif == 0)
        {
            if (head.compare_exchange_weak(pos, pos + 1,
                                           std::memory_order_relaxed))
            {
                // 读取数据
                item = slot.data;
                // 标记槽位可写
                slot.seq.store(pos + Size, std::memory_order_release);
                return true;
            }
        }
        else if (dif < 0)
        {
            // 缓冲区空
            return false;
        }
        else
        {
            pos = head.load(std::memory_order_relaxed);
        }
    }
}

} // namespace Demo1
void main()
{
    printf("Voxol::Test::LockFree::main() begin ...\n");
    printf("Voxol::Test::LockFree::main() end ...\n");
}
} // namespace Voxol::Test::LockFree