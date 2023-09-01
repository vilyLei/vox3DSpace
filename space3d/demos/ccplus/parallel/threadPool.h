#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include <stdexcept>
#include <functional>

namespace parallel::threadPool
{
namespace threadPool_01
{

void testMain()
{
    std::cout << "\threadPool_01::testMain() begin.\n";
    
    std::cout << "threadPool_01::testMain() end.\n";
}
}
void testMain()
{
    std::cout << "\nparallel::threadPool::testMain() begin.\n";
    std::thread::id thread_old_id;
    std::cout << "thread_old_id: " << thread_old_id << "\n";
    threadPool_01::testMain();
    std::cout << "parallel::threadPool::testMain() end.\n";
}
} // namespace parallel::baseNonLock2