#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <condition_variable>
#include <future>
#include <numeric>
#include <functional>
using namespace std::literals;

namespace thread
{
namespace thrFuture
{
namespace sharedFuture
{
void testSubMain01()
{
    std::promise<void>       ready_promise, t1_ready_promise, t2_ready_promise;
    std::shared_future<void> ready_future(ready_promise.get_future());

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    auto fun1 = [&, ready_future]() -> std::chrono::duration<double, std::milli> {
        t1_ready_promise.set_value();
        ready_future.wait(); // waits for the signal from main()
        std::cout << "fun1 call() ...\n";
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto fun2 = [&, ready_future]() -> std::chrono::duration<double, std::milli> {
        t2_ready_promise.set_value();
        ready_future.wait(); // waits for the signal from main()
        std::cout << "fun2 call() ...\n";
        return std::chrono::high_resolution_clock::now() - start;
    };

    auto fut1 = t1_ready_promise.get_future();
    auto fut2 = t2_ready_promise.get_future();

    auto result1 = std::async(std::launch::async, fun1);
    auto result2 = std::async(std::launch::async, fun2);

    // wait for the threads to become ready
    fut1.wait();
    fut2.wait();

    // the threads are ready, start the clock
    start = std::chrono::high_resolution_clock::now();

    // signal the threads to go
    std::cout << "shared future, ready_promise.set_value() call() ...\n";
    ready_promise.set_value();

    std::cout << "Thread 1 received the signal "
              << result1.get().count() << " ms after start\n"
              << "Thread 2 received the signal "
              << result2.get().count() << " ms after start\n";
}
void testMain()
{
    testSubMain01();
}
} // namespace sharedFuture
namespace packTask
{
// unique function to avoid disambiguating the std::pow overload set
int f(int x, int y) { return std::pow(x, y); }

void task_lambda()
{
    std::packaged_task<int(int, int)> task([](int a, int b) {
        return std::pow(a, b);
    });
    std::future<int>                  result = task.get_future();

    task(2, 9);

    std::cout << "task_lambda:\t" << result.get() << '\n';
}

void task_bind()
{
    std::packaged_task<int()> task(std::bind(f, 2, 11));
    std::future<int>          result = task.get_future();

    task();

    std::cout << "task_bind:\t" << result.get() << '\n';
}

void task_thread()
{
    std::packaged_task<int(int, int)> task(f);
    std::future<int>                  result = task.get_future();

    std::thread task_td(std::move(task), 2, 10);
    task_td.join();

    std::cout << "task_thread:\t" << result.get() << '\n';
}
int  flagFunc() { return 1; }
void task_thread01()
{
    std::packaged_task<int()> task(flagFunc);
    std::future<int>                  result = task.get_future();

    std::thread task_td(std::move(task));
    task_td.join();

    std::cout << "task_thread01:\t" << result.get() << '\n';
}

void testMain()
{
    task_lambda();
    task_bind();
    task_thread();
    task_thread01();
}
} // namespace packTask
namespace thrf01
{
void funcTSM04(std::promise<int> result_promise) noexcept
{
    //result_promise.set_value(42);
    result_promise.set_value_at_thread_exit(42);
}
void testPromiseException()
{
    std::promise<int> p;
    std::future<int>  f = p.get_future();

    std::thread t([&p] {
        try
        {
            // code that may throw
            throw std::runtime_error("Example");
        }
        catch (...)
        {
            try
            {
                // store anything thrown in the promise
                p.set_exception(std::current_exception());
                // or throw a custom exception instead
                // p.set_exception(std::make_exception_ptr(MyException("mine")));
            }
            catch (...)
            {} // set_exception() may throw too
        }
    });

    try
    {
        std::cout << f.get();
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception from the thread: " << e.what() << '\n';
    }
    t.join();
}
void testSubMain04()
{
    std::promise<int> result_promise;
    std::future<int>  result_future = result_promise.get_future();
    //std::thread       t{funcTSM04, std::move(result_promise)};
    std::thread t{
        [](std::promise<int> p) {
            std::cout << "result_promise ready set value A..." << std::endl;
            try
            {
                //p.set_value_at_thread_exit(42);// error
            p.set_value(42);
            }
            catch (...)
            {
                std::cout << "result_promise p.set_value_at_thread_exit error...\n";
            }
        },
        std::move(result_promise)};
    //t.join();
    t.detach();
    int result = result_future.get();
    std::cout << "result: " << result << "\n";

    // error
    std::promise<int> p1;
    std::future<int>  f_p1 = p1.get_future();
    std::thread       t1{
        [](std::promise<int> p) {
            std::cout << "f_p1 ready set value A..." << std::endl;
            try
            {
                //p.set_value_at_thread_exit(9);// error
                p.set_value(9);// error
            }
            catch(...)
            {
                std::cout << "f_p1 p.set_value_at_thread_exit error...\n";
            }
        },
        std::move(p1)};
    t1.detach();
    std::cout << "f_p1.get(): " << f_p1.get() << "\n";
}
void accumulate(std::vector<int>::iterator first,
                std::vector<int>::iterator last,
                std::promise<int>          accumulate_promise)
{
    int sum = std::accumulate(first, last, 0);
    accumulate_promise.set_value(sum); // Notify future
}

void do_work(std::promise<void> barrier)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    barrier.set_value();
}
void testSubMain03()
{
    std::chrono::system_clock::time_point two_seconds_passed = std::chrono::system_clock::now() + std::chrono::seconds(2);
    //std::chrono::system_clock::time_point two_seconds_passed = std::chrono::system_clock::now() + 2s;

    // Make a future that takes 1 second to complete
    // ok
    std::promise<int> p1;
    std::future<int>  f_completes = p1.get_future();
    std::thread([&p1]() {
        std::cout << "p1 ready set value A..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "p1 ready set value B..." << std::endl;
        p1.set_value_at_thread_exit(9);
    }).detach();

    // error
    //std::promise<int> p1;
    //std::future<int>  f_completes = p1.get_future();
    //std::thread([](std::promise<int> p) {
    //    std::cout << "p1 ready set value A..." << std::endl;
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
    //    std::cout << "p1 ready set value B..." << std::endl;
    //    p.set_value_at_thread_exit(9);// apply p.set_value(42) , make a success call;
    //}, std::move(p1)).detach();

    // Make a future that takes 5 seconds to complete
    std::promise<int> p2;
    std::future<int>  f_times_out = p2.get_future();
    std::thread([](std::promise<int> p2) {
        std::cout << "p2 ready set value A..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        //p2.set_value_at_thread_exit(8);
        p2.set_value(8);
    },
                std::move(p2))
        .detach();

    std::cout << "Waiting for 2 seconds..." << std::endl;

    if (std::future_status::ready == f_completes.wait_until(two_seconds_passed))
    {
        std::cout << "f_completes ready get value A..." << std::endl;
        std::cout << "f_completes: " << f_completes.get() << "\n";
    }
    else
    {
        std::cout << "f_completes did not complete!\n";
    }

    if (std::future_status::ready == f_times_out.wait_until(two_seconds_passed))
    {
        std::cout << "f_times_out: " << f_times_out.get() << "\n";
    }
    else
    {
        std::cout << "f_times_out did not complete!\n";
    }
    for (int i = 0; i < 3; ++i)
    {
        std::this_thread::sleep_for(1s);
    }
    std::cout << "Done!\n";
}
void testSubMain02()
{
    std::future<int> future = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(3s);
        return 8;
    });

    std::cout << "waiting...\n";
    std::future_status status;
    do {
        switch (status = future.wait_for(1s); status)
        {
            case std::future_status::deferred: std::cout << "deferred\n"; break;
            case std::future_status::timeout: std::cout << "timeout\n"; break;
            case std::future_status::ready: std::cout << "ready!\n"; break;
        }
    } while (status != std::future_status::ready);

    std::cout << "result is " << future.get() << '\n';
}
void testSubMain01()
{
    // future from a packaged_task
    std::packaged_task<int()> task([] { return 7; }); // wrap the function
    std::future<int>          f1 = task.get_future(); // get a future
    std::thread               t(std::move(task));     // launch on a thread

    // future from an async()
    std::future<int> f2 = std::async(std::launch::async, [] { return 8; });

    // future from a promise
    std::promise<int> p;
    std::future<int>  f3 = p.get_future();
    std::thread([&p] {
        std::cout << "\npromise calling ...\n"
                  << std::flush;
        std::this_thread::sleep_for(1s);
        p.set_value_at_thread_exit(9);
        // twice ops, error.
        //p.set_value_at_thread_exit(9);
    }).detach();
    std::cout << "Waiting..." << std::flush;
    auto needReady = false;
    if (needReady)
    {
        f1.wait();
        f2.wait();
        f3.wait();
    }
    std::cout << "Done!\nResults are: "
              << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
    //t.join();
    t.detach();

    for (int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(50ms);
    }
}
void testMain()
{
    std::cout << "thread::thrFuture::thrf01::testMain() begin.\n";

    //testSubMain01();
    //testSubMain02();
    //testSubMain03();
    testSubMain04();
    return;

    // Demonstrate using promise<int> to transmit a result between threads.
    std::vector<int>  numbers = {1, 2, 3, 4, 5, 6};
    std::promise<int> accumulate_promise;
    std::future<int>  accumulate_future = accumulate_promise.get_future();
    std::thread       work_thread(accumulate, numbers.begin(), numbers.end(),
                                  std::move(accumulate_promise));

    // future::get() will wait until the future has a valid result and retrieves it.
    // Calling wait() before get() is not needed
    //accumulate_future.wait();  // wait for result
    std::cout << "result=" << accumulate_future.get() << '\n';
    work_thread.join(); // wait for thread completion

    // Demonstrate using promise<void> to signal state between threads.
    std::promise<void> barrier;
    std::future<void>  barrier_future = barrier.get_future();
    std::thread        new_work_thread(do_work, std::move(barrier));
    barrier_future.wait();
    new_work_thread.join();
    std::cout << "thread::thrFuture::thrf01::testMain() end.\n";
}
} // namespace thrf01
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::thrFuture::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //thrf01::testMain();
    sharedFuture::testMain();
    std::cout << "thread::thrFuture::testMain() end.\n";
}
} // namespace thrFuture
} // namespace thread