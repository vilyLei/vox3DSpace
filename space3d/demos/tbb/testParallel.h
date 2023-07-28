
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <chrono>

#include "tbb/tbb.h"
#include "tbb/spin_mutex.h"
#include "oneapi/tbb/parallel_for.h"
#include "oneapi/tbb/task_arena.h"
#include "oneapi/tbb/blocked_range.h"
#include "oneapi/tbb/tick_count.h"

namespace testParallel
{
static const std::size_t N = 16;

void SerialSubStringFinder(const std::string&        str,
                           std::vector<std::size_t>& max_array,
                           std::vector<std::size_t>& pos_array)
{
    for (std::size_t i = 0; i < str.size(); ++i)
    {
        std::size_t max_size = 0, max_pos = 0;
        for (std::size_t j = 0; j < str.size(); ++j)
            if (j != i)
            {
                std::size_t limit = str.size() - (std::max)(i, j);
                for (std::size_t k = 0; k < limit; ++k)
                {
                    if (str[i + k] != str[j + k])
                        break;
                    if (k > max_size)
                    {
                        max_size = k;
                        max_pos  = j;
                    }
                }
            }
        max_array[i] = max_size;
        pos_array[i] = max_pos;
    }
}

class SubStringFinder
{
    const char*       str;
    const std::size_t len;
    std::size_t*      max_array;
    std::size_t*      pos_array;

public:
    void operator()(const oneapi::tbb::blocked_range<std::size_t>& r) const
    {
        for (std::size_t i = r.begin(); i != r.end(); ++i)
        {
            std::size_t max_size = 0, max_pos = 0;
            for (std::size_t j = 0; j < len; ++j)
            {
                if (j != i)
                {
                    std::size_t limit = len - (std::max)(i, j);
                    for (std::size_t k = 0; k < limit; ++k)
                    {
                        if (str[i + k] != str[j + k])
                            break;
                        if (k > max_size)
                        {
                            max_size = k;
                            max_pos  = j;
                        }
                    }
                }
            }
            max_array[i] = max_size;
            pos_array[i] = max_pos;
        }
    }
    // We do not use std::vector for compatibility with offload execution
    SubStringFinder(const char* s, const std::size_t s_len, std::size_t* m, std::size_t* p) :
        str(s),
        len(s_len),
        max_array(m),
        pos_array(p) {}
};

int main_01()
{
    auto max_concurrency = oneapi::tbb::this_task_arena::max_concurrency();

    std::cout << "max_concurrency: " << max_concurrency << std::endl;

    std::string str[N] = {std::string("a"), std::string("b")};
    for (std::size_t i = 2; i < N; ++i)
        str[i] = str[i - 1] + str[i - 2];
    std::string&      to_scan  = str[N - 1];
    const std::size_t num_elem = to_scan.size();

    std::vector<std::size_t> max1(num_elem);
    std::vector<std::size_t> pos1(num_elem);
    std::vector<std::size_t> max2(num_elem);
    std::vector<std::size_t> pos2(num_elem);

    std::cout << " Done building string."
              << "\n";

    oneapi::tbb::tick_count serial_t0 = oneapi::tbb::tick_count::now();
    SerialSubStringFinder(to_scan, max2, pos2);
    oneapi::tbb::tick_count serial_t1 = oneapi::tbb::tick_count::now();
    std::cout << " Done with serial version."
              << "\n";

    oneapi::tbb::tick_count parallel_t0 = oneapi::tbb::tick_count::now();
    oneapi::tbb::parallel_for(oneapi::tbb::blocked_range<std::size_t>(0, num_elem, 100),
                              SubStringFinder(to_scan.c_str(), num_elem, &max1[0], &pos1[0]));
    oneapi::tbb::tick_count parallel_t1 = oneapi::tbb::tick_count::now();
    std::cout << " Done with parallel version."
              << "\n";

    for (std::size_t i = 0; i < num_elem; ++i)
    {
        if (max1[i] != max2[i] || pos1[i] != pos2[i])
        {
            std::cout << "ERROR: Serial and Parallel Results are Different!"
                      << "\n";
            break;
        }
    }
    std::cout << " Done validating results."
              << "\n";

    std::cout << "Serial version ran in " << (serial_t1 - serial_t0).seconds() << " seconds"
              << "\n"
              << "Parallel version ran in " << (parallel_t1 - parallel_t0).seconds() << " seconds"
              << "\n"
              << "Resulting in a speedup of "
              << (serial_t1 - serial_t0).seconds() / (parallel_t1 - parallel_t0).seconds() << "\n";

    return 0;
}

class say_hello
{
    const char* id;

public:
    say_hello(const char* s) :
        id(s) {}

    void operator()() const
    {
        std::cout<<"hello from task" << id << std::endl;
    }
};

int main2()
{
    tbb::spin_rw_mutex lock;
    //tbb::spin_mutex lock;
    std::cout << "tbb task begin." <<std::endl;
    tbb::task_group tg;
    tg.run(say_hello("1")); // spawn 1st task and return
    tg.run(say_hello("2")); // spawn 2nd task and return
    tg.wait();              // wait for tasks to complete
    std::cout << "tbb task end." << std::endl;
    return EXIT_SUCCESS;
}

int a = 0;

tbb::spin_rw_mutex mtx;


std::atomic<unsigned> counter;
unsigned         GetUniqueInterger()
{
    return std::atomic_fetch_add(&counter, 1);
}

// 如果处理器支持HTM，那么请使用 tbb::speculative_spin_rw_mutex mtx;
void read1()
{
    tbb::spin_rw_mutex::scoped_lock rlock;
    rlock.acquire(mtx, false); // inputting false represents reading lock  传入false表示这是一个读锁
    std::cout << a << std::endl;
    rlock.release();
}

void write1()
{
    tbb::spin_rw_mutex::scoped_lock wlock;
    wlock.acquire(mtx); // default writing lock 默认为写锁
    for (int i = 0; i < 100000; i++) a++;
    wlock.release();
}

int main()
{
    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(10));

    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back(write1);
    }
    for (auto& t : threads)
    {
        t.join();
    }
    read1();
    return EXIT_SUCCESS;
}
void testMain()
{
}
}