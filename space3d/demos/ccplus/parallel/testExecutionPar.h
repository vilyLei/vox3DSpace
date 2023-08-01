#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <condition_variable>
#include <future>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <random>

using vector_uint64_t = std::vector<std::uint64_t>;

// #define PARALLEL
namespace parallel
{
namespace execParallel
{

namespace execParDemo
{

#ifdef PARALLEL
#include <execution>
namespace execution = std::execution;
#else
enum class execution
{
    seq,
    unseq,
    par_unseq,
    par
};
#endif

void measure([[maybe_unused]] auto policy, vector_uint64_t v)
{
    const auto start = std::chrono::steady_clock::now();
#ifdef PARALLEL
    std::sort(policy, v.begin(), v.end());
#else
    std::sort(v.begin(), v.end());
#endif
    const auto finish = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
              << '\n';
};

int testMain()
{
    using namespace std;
    std::cout << "parallel::execParallel::execParDemo::testMain() begin.\n";
    auto v_size = 1'000'000;
    std::cout << "v_size: " << v_size << "\n";
    vector_uint64_t v(1'000'000);
    std::mt19937               gen{std::random_device{}()};
    std::ranges::generate(v, gen);

    measure(execution::seq, v);
    measure(execution::unseq, v);
    measure(execution::par_unseq, v);
    measure(execution::par, v);
    std::cout << "parallel::execParallel::execParDemo::testMain() begin.\n";
    return 1;
}
} // namespace async_demo_1
void testMain()
{
    std::cout << "__cplusplus: " << __cplusplus << "\n";
    std::boolalpha(std::cout);
    std::cout << "parallel::execParallel::testMain() begin.\n";

    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    execParDemo::testMain();
    std::cout << "parallel::execParallel::testMain() end.\n";
}
} // namespace execParallel
} // namespace thread