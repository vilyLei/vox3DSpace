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
namespace atomic_wait
{
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std::literals;

int testMain()
{
    std::atomic<bool>     all_tasks_completed{false};
    std::atomic<unsigned> completion_count{};
    std::future<void>     task_futures[16];
    std::atomic<unsigned> outstanding_task_count{16};

#if __cplusplus >= 202002L
    // Spawn several tasks which take different amounts of
    // time, then decrement the outstanding task count.
    for (std::future<void>& task_future : task_futures)
    {
        task_future = std::async([&] {
            // This sleep represents doing real work...
            std::this_thread::sleep_for(50ms);

            ++completion_count;
            --outstanding_task_count;

            // When the task count falls to zero, notify
            // the waiter (main thread in this case).
            if (outstanding_task_count.load() == 0)
            {
                all_tasks_completed = true;
                all_tasks_completed.notify_one();
            }
        });
    }

    all_tasks_completed.wait(false);
#endif

    std::cout << "Tasks completed = " << completion_count.load() << '\n';
    return 0;
}
} // namespace atomic_wait
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
} // namespace async_demo_1
void testMain()
{
    // thanks: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
    std::cout << "__cplusplus: " << __cplusplus << "\n";
    std::boolalpha(std::cout);
    std::cout << "thread::asyncDemo::testMain() begin.\n";

    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //async_demo_1::testMain();
    atomic_wait::testMain();
    std::cout << "thread::asyncDemo::testMain() end.\n";
}
} // namespace asyncDemo
} // namespace thread