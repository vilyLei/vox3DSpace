#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>

using namespace std::literals;

namespace thread
{
namespace memoryFence
{
namespace memoryFence_t01
{
std::atomic<bool> x, y;
std::atomic<int> z;
void write_x_then_y()
{
    //std::this_thread::sleep_for(10ms);
    x.store(true, std::memory_order_relaxed);            // 1
    std::atomic_thread_fence(std::memory_order_release); // 2
    y.store(true, std::memory_order_relaxed);            // 3
}
void read_y_then_x()
{
    //std::this_thread::sleep_for(10ms);
    while(!y.load(std::memory_order_relaxed))
        ;                                                // 4
    std::atomic_thread_fence(std::memory_order_acquire); // 5
    if (x.load(std::memory_order_relaxed))               // 6
        ++z;
}
void testMain()
{
    std::cout << "memoryFence_t01::testMain(), begin() ...\n";
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() != 0); // 7
    std::cout << "memoryFence_t01::testMain(), end() ...\n";
}
} // namespace memoryFence_t01
void testMain()
{
    std::cout << "thread::memoryFence::testMain(), begin() ...\n";
    memoryFence_t01::testMain();
    std::cout << "thread::memoryFence::testMain(), end() ...\n";
}
} // namespace memoryFence
} // namespace thread