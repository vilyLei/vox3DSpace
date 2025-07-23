#include "TestLockFree.h"
#include <cstdio>
#include <atomic>
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

} // namespace Demo1
void main()
{
    printf("Voxol::Test::LockFree::main() begin ...\n");
    printf("Voxol::Test::LockFree::main() end ...\n");
}
} // namespace Voxol::Test::LockFree