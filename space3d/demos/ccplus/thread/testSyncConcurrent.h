#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <condition_variable>
#include <future>
#include <numeric>
using namespace std::literals;

namespace thread
{
namespace syncConcurrent
{
namespace test_2
{
class ChunkParser
{
public:
    //std::thread_local int stride = 0;
    thread_local static int Stride;
    ChunkParser() {}
    void operator()()
    {
        std::cout << "ChunkParser() thread call() ...\n";
        using namespace std::literals;
        std::this_thread::sleep_for(700ms);
    }
};
thread_local int ChunkParser::Stride = 0;

void testMain()
{
    std::vector<std::thread> thrs;
    for (auto i = 0; i < 3; ++i)
    {
        thrs.emplace_back(ChunkParser());
    }
    for (auto& v : thrs)
    {
        v.join();
    }
}
} // namespace test_2
namespace promiseDemo_1
{
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

int testMain()
{
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
    return 1;
}
} // namespace promiseDemo_1
namespace futureDemo_2
{
int testMain()
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
            }
            catch (...)
            {} // set_exception() may throw too
        }
    });

    try
    {
        std::cout << "f.get(): " << f.get() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception from the thread: " << e.what() << '\n';
    }
    t.join();
    return 1;
}
} // namespace futureDemo_2
namespace futureDemo_1
{

int testMain()
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
    std::thread([&p] { p.set_value_at_thread_exit(9); }).detach();

    std::cout << "Waiting..." << std::flush;
    f1.wait();
    f2.wait();
    f3.wait();
    std::cout << "Done!\nResults are: "
              << f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';
    t.join();
    return 1;
}
} // namespace futureDemo_1
namespace condition_variable_demo_notify_all
{
//std::condition_variable_any cv;
std::condition_variable_any cv;
std::mutex                  cv_m; // This mutex is used for three purposes:
                                  // 1) to synchronize accesses to i
                                  // 2) to synchronize accesses to std::cerr
                                  // 3) for the condition variable cv
int i = 0;

void waits()
{
    std::unique_lock<std::mutex> lk(cv_m);
    //std::lock_guard lk(cv_m);
    std::cerr << "Waiting... \n";
    cv.wait(lk, [] { return i == 1; });
    std::cerr << "...finished waiting. i == 1\n";
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lk(cv_m);
        std::cerr << "Notifying...\n";
    }
    cv.notify_all();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        std::lock_guard<std::mutex> lk(cv_m);
        i = 1;
        std::cerr << "Notifying again...\n";
    }
    cv.notify_all();
}

int testMain()
{
    std::thread t1(waits), t2(waits), t3(waits), t4(signals);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 1;
}
} // namespace condition_variable_demo_notify_all
namespace condition_variable_demo
{

std::mutex              m;
std::condition_variable cv;
std::condition_variable cv02;
std::string             data;
bool                    ready     = false;
bool                    processed = false;

void worker_thread()
{
    std::cout << "Worker thread worker_thread().\n";
    // Wait until main() sends data
    std::unique_lock lk(m);
    cv02.wait(lk, [] { return ready; });

    // after the wait, we own the lock.
    std::cout << "Worker thread is processing data\n";
    data += " after processing";

    // Send data back to main()
    processed = true;
    std::cout << "Worker thread signals data processing completed\n";

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lk.unlock();
    cv.notify_one();
}
void notifyReadyFlag()
{
    std::cout << "notifyReadyFlag() begin.\n";
    for (auto i = 0; i < 3; ++i)
    {
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(300ms));
    }
    std::unique_lock lk(m);
    ready = true;
    //processed = true;
    lk.unlock();
    //cv.notify_one();
    cv02.notify_one();
    std::cout << "notifyReadyFlag() end.\n";
}
int testMain()
{
    // thanks: https://en.cppreference.com/w/cpp/thread/condition_variable
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard lk(m);
        //ready = true;
        std::cout << "main() signals data ready for processing\n";
    }
    //cv.notify_one();
    //
    std::thread notifyWorker(notifyReadyFlag);
    notifyWorker.detach();
    // wait for the worker
    {
        std::unique_lock lk(m);
        cv.wait(lk, [] { return processed; });
    }
    std::cout << "Back in main(), data = " << data << '\n';
    //

    worker.join();

    return 1;
}
} // namespace condition_variable_demo
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::syncConcurrent::testMain() begin.\n\n";

    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //condition_variable_demo::testMain();
    condition_variable_demo_notify_all::testMain();
    //futureDemo_2::testMain();
    //test_2::testMain();
    std::cout << "thread::syncConcurrent::testMain() end.\n\n";
}
} // namespace syncConcurrent
} // namespace thread