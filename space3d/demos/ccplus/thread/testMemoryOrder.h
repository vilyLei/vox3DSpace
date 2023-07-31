#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>

namespace thread
{
namespace memoryOrder
{
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::memoryOrder::testMain().\n";
    std::cout << std::atomic<int>::is_always_lock_free << "\n";
}
} // namespace memoryOrder
} // namespace thread