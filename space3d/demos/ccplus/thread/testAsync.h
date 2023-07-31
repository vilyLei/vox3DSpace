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

namespace thread
{
namespace asyncDemo
{
namespace async_demo_1
{
std::mutex m;

struct X
{
    void foo(int i, const std::string& str)
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << str << ' ' << i << '\n';
    }

    void bar(const std::string& str)
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "x::bar() str: " << str << '\n';
    }

    int operator()(int i)
    {
        std::lock_guard<std::mutex> lk(m);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << i << '\n';
        return i + 10;
    }
};

template <typename RandomIt>
int parallel_sum(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < 1000)
        return std::accumulate(beg, end, 0);

    RandomIt mid    = beg + len / 2;
    auto     handle = std::async(std::launch::async,
                                 parallel_sum<RandomIt>, mid, end);
    int      sum    = parallel_sum(beg, mid);
    return sum + handle.get();
}

int testMain()
{
    std::cout << "thread::asyncDemo::async_demo_1::testMain() begin.\n";
    std::vector<int> v(10000, 1);
    std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';

    X x;
    // Calls (&x)->foo(42, "Hello") with default policy:
    // may print "Hello 42" concurrently or defer execution
    auto a1 = std::async(&X::foo, &x, 42, "Hello");
    // Calls x.bar("world!") with deferred policy
    // prints "world!" when a2.get() or a2.wait() is called
    auto a2 = std::async(std::launch::deferred, &X::bar, x, "world!");
    // Calls X()(43); with async policy
    // prints "43" concurrently
    auto a3 = std::async(std::launch::async, X(), 43);
    a2.wait();                     // prints "world!"
    std::cout << a3.get() << '\n'; // prints "53"
    std::cout << "thread::asyncDemo::async_demo_1::testMain() end.\n";
    return 1;
} // if a1 is not done at this point, destructor of a1 prints "Hello 42" here
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::asyncDemo::testMain() begin.\n";

    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    async_demo_1::testMain();
    std::cout << "thread::asyncDemo::testMain() end.\n";
}
} // namespace syncConcurrent
} // namespace thread