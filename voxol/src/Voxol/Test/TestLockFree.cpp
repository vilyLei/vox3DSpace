#include "TestLockFree.h"
#include <cstdio>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <cassert>

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

// Single Producer Single Consumer
template <typename T, size_t N>
class SPSCQueue
{
    T      buffer[N];
    size_t head = 0;
    size_t tail = 0;

public:
    bool enqueue(const T& val)
    {
        size_t next = (tail + 1) % N;
        if (next == head) return false; // full
        buffer[tail] = val;
        tail         = next;
        return true;
    }

    bool dequeue(T& val)
    {
        if (head == tail) return false; // empty
        val  = buffer[head];
        head = (head + 1) % N;
        return true;
    }
};

// MPSC（Multi-Producer Single-Consumer）
template <typename T>
class MPSCQueue {
private:
    struct Node {
        std::unique_ptr<Node> next;
        T value;
        Node(const T& val) : value(val) {}
    };

    std::atomic<Node*> tail;    // 多生产者写入
    Node* head;                 // 单消费者读取

public:
    MPSCQueue() {
        Node* dummy = new Node(T{});  // dummy 节点
        head = dummy;
        tail.store(dummy, std::memory_order_relaxed);
    }

    ~MPSCQueue() {
        while (dequeue());  // 清理所有元素
        delete head;        // 删除 dummy 节点
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = nullptr;

        Node* prev = tail.exchange(newNode, std::memory_order_acq_rel);
        prev->next.reset(newNode);  // 由单线程消费者读取，不用原子
    }

    // 单线程调用
    bool dequeue(T* out = nullptr) {
        std::unique_ptr<Node> next = std::move(head->next);
        if (!next) return false;

        if (out) *out = next->value;

        Node* old = head;
        head = next.release();
        delete old;  // 删除旧 head

        return true;
    }
};
int mpscTestMain() {
    MPSCQueue<int> queue;

    // 多个生产者线程
    std::vector<std::thread> producers;
    for (int i = 0; i < 4; ++i) {
        producers.emplace_back([&queue, i]() {
            for (int j = 0; j < 5; ++j) {
                queue.enqueue(i * 10 + j);
            }
        });
    }

    for (auto& p : producers) p.join();

    // 单个消费者线程
    int val;
    while (queue.dequeue(&val)) {
        std::cout << "Got: " << val << std::endl;
    }

    return 0;
}
} // namespace Demo1
namespace DemoMPSC
{
    
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

    // struct alignas(64) Slot
    // {
    //     std::atomic<size_t> seq;
    //     T                   data;
    //     char                padding[64 - sizeof(std::atomic<size_t>) - sizeof(T)];
    // };
    // alignas(64) Slot buffer[Size];
    // union alignas(64) align_tail
    // {
    //     std::atomic<size_t> tail;
    // };
    // union alignas(64) align_head
    // {
    //     std::atomic<size_t> head;
    // };
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
constexpr int NUM_PRODUCERS = 4;
constexpr int NUM_CONSUMERS = 2;
constexpr int ITEMS_PER_PRODUCER = 100000;

MPMCQueue<int, 1024> queue; // 环形队列大小，必须是2的幂，内部处理 wrap-around

std::atomic<int> produced_count{0};
std::atomic<int> consumed_count{0};

void producer(int id) {
    for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        int value = id * ITEMS_PER_PRODUCER + i;
        while (!queue.enqueue(value)) {
            // 队列满，稍作等待
            std::this_thread::yield();
        }
        produced_count.fetch_add(1, std::memory_order_relaxed);
    }
}

void consumer(int id) {
    int local_count = 0;
    while (true) {
        int value;
        if (queue.dequeue(value)) {
            ++local_count;
            consumed_count.fetch_add(1, std::memory_order_relaxed);
        } else {
            if (produced_count.load(std::memory_order_relaxed) >= NUM_PRODUCERS * ITEMS_PER_PRODUCER) {
                break;
            }
            std::this_thread::yield();
        }
    }
    std::cout << "[Consumer " << id << "] consumed " << local_count << " items\n";
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(consumer, i);
    }

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer, i);
    }

    for (auto& p : producers) p.join();
    for (auto& c : consumers) c.join();

    std::cout << "Produced: " << produced_count.load() << "\n";
    std::cout << "Consumed: " << consumed_count.load() << "\n";
    assert(produced_count.load() == consumed_count.load());

    return 0;
}
}
void main()
{
    printf("Voxol::Test::LockFree::main() begin ...\n");
    printf("Voxol::Test::LockFree::main() end ...\n");
}
} // namespace Voxol::Test::LockFree