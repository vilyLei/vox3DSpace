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
#include <compare>
#include <atomic>
#include <cstddef>
#include <mutex>
#include <new>
#include <iomanip>

namespace parallel::hardware
{
namespace hardware_p01
{

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// 在 x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ... 上为 64 字节
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size  = 64;
#endif

std::mutex cout_mutex;

constexpr int max_write_iterations{10'000'000}; // 性能评估时间调节

struct alignas(hardware_constructive_interference_size)
    OneCacheLiner
{   // 两个原子变量总共占据一条缓存线(cpu cache line)
    std::atomic_uint64_t x{};
    std::atomic_uint64_t y{};
} oneCacheLiner;

struct TwoCacheLiner
{ // 两个原子变量各自独立占据一条缓存线，总共使用了两条缓存线(cpu cache line)
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t x{};
    alignas(hardware_destructive_interference_size) std::atomic_uint64_t y{};
} twoCacheLiner;

inline auto now() noexcept { return std::chrono::high_resolution_clock::now(); }

template <bool xy>
void oneCacheLinerThread()
{
    const auto start{now()};

    for (uint64_t count{}; count != max_write_iterations; ++count)
        if constexpr (xy)
            oneCacheLiner.x.fetch_add(1, std::memory_order_relaxed);
        else
            oneCacheLiner.y.fetch_add(1, std::memory_order_relaxed);

    const std::chrono::duration<double, std::milli> elapsed{now() - start};
    std::lock_guard                                 lk{cout_mutex};
    std::cout << "oneCacheLinerThread() spent " << elapsed.count() << " ms\n";
    if constexpr (xy)
        oneCacheLiner.x = elapsed.count();
    else
        oneCacheLiner.y = elapsed.count();
}

template <bool xy>
void twoCacheLinerThread()
{
    const auto start{now()};

    for (uint64_t count{}; count != max_write_iterations; ++count)
        if constexpr (xy)
            twoCacheLiner.x.fetch_add(1, std::memory_order_relaxed);
        else
            twoCacheLiner.y.fetch_add(1, std::memory_order_relaxed);

    const std::chrono::duration<double, std::milli> elapsed{now() - start};
    std::lock_guard                                 lk{cout_mutex};
    std::cout << "twoCacheLinerThread() spent " << elapsed.count() << " ms\n";
    if constexpr (xy)
        twoCacheLiner.x = elapsed.count();
    else
        twoCacheLiner.y = elapsed.count();
}

void testMain()
{
    std::atomic_uint64_t ap_01{};
    std::cout << "sizeof(int): " << sizeof(int) << " bytes\n";
    std::cout << "sizeof(long): " << sizeof(long) << " bytes\n";
    std::cout << "sizeof(long long): " << sizeof(long long) << " bytes\n";
    std::cout << "sizeof(std::atomic_uint64_t): " << sizeof(std::atomic_uint64_t) << " bytes\n";
    std::cout << "\n";
    std::cout << "sizeof(oneCacheLiner.x): " << sizeof(oneCacheLiner.x) << " bytes\n";
    std::cout << "sizeof(oneCacheLiner.y): " << sizeof(oneCacheLiner.y) << " bytes\n";
    std::cout << "sizeof(oneCacheLiner): " << sizeof(oneCacheLiner) << " bytes\n";
    std::cout << "\n";
    std::cout << "sizeof(twoCacheLiner.x): " << sizeof(twoCacheLiner.x) << " bytes\n";
    std::cout << "sizeof(twoCacheLiner.y): " << sizeof(twoCacheLiner.y) << " bytes\n";
    std::cout << "sizeof(twoCacheLiner): " << sizeof(twoCacheLiner) << " bytes\n";
    std::cout << "\n";
    // 获得当前系统cpu核心数量
    auto cupCount = std::thread::hardware_concurrency();
    std::cout << "cpu核心数量: " << cupCount << "\n";
    std::cout << "\n";

    std::cout << "__cpp_lib_hardware_interference_size "
#ifdef __cpp_lib_hardware_interference_size
                 " = "
              << __cpp_lib_hardware_interference_size << "\n";
#else
                 "is not defined, use 64 as fallback\n";
#endif

    std::cout
        << "hardware_destructive_interference_size == "
        << hardware_destructive_interference_size << '\n'
        << "hardware_constructive_interference_size == "
        << hardware_constructive_interference_size << "\n\n";

    std::cout
        << std::fixed << std::setprecision(2)
        << "sizeof( OneCacheLiner ) == " << sizeof(OneCacheLiner) << '\n'
        << "sizeof( TwoCacheLiner ) == " << sizeof(TwoCacheLiner) << "\n\n";

    constexpr int max_runs{4};

    int oneCacheLiner_average{0};
    for (auto i{0}; i != max_runs; ++i)
    {
        std::thread th1{oneCacheLinerThread<0>};
        std::thread th2{oneCacheLinerThread<1>};
        th1.join();
        th2.join();
        oneCacheLiner_average += oneCacheLiner.x + oneCacheLiner.y;
    }
    std::cout << "Average time: " << (oneCacheLiner_average / max_runs / 2) << " ms\n\n";

    int twoCacheLiner_average{0};
    for (auto i{0}; i != max_runs; ++i)
    {
        std::thread th1{twoCacheLinerThread<0>};
        std::thread th2{twoCacheLinerThread<1>};
        th1.join();
        th2.join();
        twoCacheLiner_average += twoCacheLiner.x + twoCacheLiner.y;
    }
    std::cout << "Average time: " << (twoCacheLiner_average / max_runs / 2) << " ms\n\n";
}
} // namespace hardware_p01
void testMain()
{
    std::cout << "\nparallel::hardware_p01::testMain() begin.\n";
    hardware_p01::testMain();
    std::cout << "parallel::hardware_p01::testMain() end.\n";
}
} // namespace parallel::hardware