#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>

namespace thread
{
namespace memoryOrder
{
// Release-Consume ordering
namespace memoryOrder_t04
{

std::atomic<std::string*> ptr;
int                       data;

void producer()
{
    std::string* p = new std::string("Hello");
    data           = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)))
        ;
    assert(*p2 == "Hello"); // 绝无出错： *p2 从 ptr 携带依赖
    assert(data == 42);     // 可能也可能不会出错： data 不从 ptr 携带依赖
    std::cout << "memoryOrder_t04() consumer() ... data: " << data << "\n";
}

void testMain()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
} // namespace memoryOrder_t04
// Release-Acquire ordering
namespace memoryOrder_t03
{
std::vector<int> data;
std::atomic<int> flag = {0};

void thread_1()
{
    data.push_back(42);
    flag.store(1, std::memory_order_release);
}

void thread_2()
{
    int expected = 1;
    while (!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
    {
        expected = 1;
    }
    data.push_back(43);
}

void thread_3()
{
    while (flag.load(std::memory_order_acquire) < 2)
        ;
    //assert(data.at(0) == 42); // 决不出错
    assert(data[0] == 42); // 决不出错

    std::cout << "thread_3() call() ... data.size(): " << data.size() << "\n";
    if (data.size() > 1)
    {
        std::cout << "thread_3() data[1]: " << data[1] << "\n";
    }
}

void testMain()
{
    std::thread a(thread_1);
    std::thread b(thread_2);
    std::thread c(thread_3);
    a.join();
    b.join();
    c.join();
}
} // namespace memoryOrder_t03
// Release-Acquire ordering
namespace memoryOrder_t02
{
std::atomic<std::string*> ptr;
int                       data;

void producer()
{
    std::string* p = new std::string("Hello");
    data           = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_acquire)))
        ;
    assert(*p2 == "Hello"); // 绝无问题
    assert(data == 42);     // 绝无问题
}

void testMain()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
} // namespace memoryOrder_t02
// Relaxed ordering
namespace memoryOrder_t01
{

std::atomic<int> cnt = {0};

void f()
{
    for (int n = 0; n < 1000; ++n)
    {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}
void testMain()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(f);
    }
    for (auto& t : v)
    {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n';
}
} // namespace memoryOrder_t01
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::memoryOrder::testMain(), begin() ...\n";
    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //memoryOrder_t01::testMain();
    //memoryOrder_t02::testMain();
    //memoryOrder_t03::testMain();
    memoryOrder_t04::testMain();
    std::cout << "thread::memoryOrder::testMain(), end() ...\n";
}
} // namespace memoryOrder
} // namespace thread