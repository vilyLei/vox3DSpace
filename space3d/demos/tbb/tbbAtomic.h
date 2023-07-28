#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>

namespace tbbAtomic
{
#define ATOMIC_TEST 1
#if (ATOMIC_TEST == 1)
std::atomic<int> flagValue;
#else
int flagValue{0};
#endif
void little_sleep(std::chrono::microseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end   = start + us;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}
void thrCall(void)
{
    for (int i = 0; i < 100; i++)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
        //std::this_thread::yield();
        little_sleep(std::chrono::microseconds(1000));
        flagValue += 1;
    }
    std::cout << "thrCall() print ...\n";
}
void testMain()
{
    std::cout << "tbbAtomic testMain begin ...\n";
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread(thrCall));
    }
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    std::cout << "flagValue: " << flagValue << "\n";
    std::cout << "tbbAtomic testMain end ...\n";
}
} // namespace tbbAtomic