#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>


namespace tbbAtomic2
{
#define ATOMIC_TEST       1
#define ATOMIC_MUTEX_TEST 0
#if (ATOMIC_TEST == 1)
std::atomic<int> flagValue;
#else
int flagValue{0};
#endif

tbb::spin_rw_mutex atomicTestMtx;
void               little_sleep(std::chrono::microseconds us)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end   = start + us;
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}
void thrCall(void)
{
    auto temp = 0;
    for (int i = 0; i < 10000; i++)
    {
        temp += 1;
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //std::this_thread::yield();
    //little_sleep(std::chrono::microseconds(1000));
#if (ATOMIC_MUTEX_TEST == 1)

    tbb::spin_rw_mutex::scoped_lock wlock;
    wlock.acquire(atomicTestMtx);
    flagValue += temp;
    wlock.release();
#else
    flagValue += temp;
#endif
    //std::cout << "thrCall() print ...\n";
}
void testMain()
{
    std::cout << "tbbAtomic2 testMain begin ...\n";
    auto start_sys_time = std::chrono::system_clock::now();
    auto start_time     = std::chrono::high_resolution_clock::now();
    //time.
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < 10; ++i)
    {
        threads.push_back(std::thread(thrCall));
    }
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    auto current_time     = std::chrono::high_resolution_clock::now();
    auto elapsedTime      = current_time - start_time;
    auto current_sys_time = std::chrono::system_clock::now();
    auto elapsedSysTime   = current_sys_time - start_sys_time;

    std::cout << "loss time: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count() << "us\n";
    std::cout << "loss sys time: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsedSysTime).count() << "us\n";
    std::cout << "flagValue: " << flagValue << "\n";
    std::cout << "tbbAtomic2 testMain end ...\n";
}
} // namespace tbbAtomic2