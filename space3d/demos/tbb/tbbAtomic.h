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
void thrCall(void)
{
    for (int i = 0; i < 100000; i++)
    {
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