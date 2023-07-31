#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace thread
{
namespace mutex
{
namespace call_once_demo
{

std::once_flag flag1, flag2;

void simple_do_once()
{
    std::call_once(flag1, []() { std::cout << "Simple example: called once\n"; });
}

void may_throw_function(bool do_throw)
{
    if (do_throw)
    {
        std::cout << "throw: call_once will retry\n"; // this may appear more than once
        throw std::exception();
    }
    std::cout << "Did not throw, call_once will not attempt again\n"; // guaranteed once
}

void do_once(bool do_throw)
{
    try
    {
        std::call_once(flag2, may_throw_function, do_throw);
    }
    catch (...)
    {}
}

int testMain()
{
    std::cout << "thread::mutex::call_once_demo::testMain() begin.\n";
    std::thread st1(simple_do_once);
    std::thread st2(simple_do_once);
    std::thread st3(simple_do_once);
    std::thread st4(simple_do_once);
    st1.join();
    st2.join();
    st3.join();
    st4.join();

    std::cout << "thread::mutex::call_once_demo::testMain() step 1.\n";

    std::thread t1(do_once, true);
    std::thread t2(do_once, true);
    std::thread t3(do_once, false);
    std::thread t4(do_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    std::cout << "thread::mutex::call_once_demo::testMain() end.\n";
    return 1;
}
}
namespace lockGuard
{
std::list<bool> flag_list;
std::list<int> some_list;  // 1
std::mutex     some_mutex; // 2
void           add_to_list(int new_value)
{
    //std::lock_guard<std::mutex> guard(some_mutex); // 3
    //std::scoped_lock<std::mutex> guard(some_mutex); // 3
    some_list.push_back(new_value);
}
bool list_contains(int value_to_find)
{
    //std::lock_guard<std::mutex> guard(some_mutex); // 4
    //std::scoped_lock<std::mutex> guard(some_mutex); // 4
    return std::find(some_list.begin(), some_list.end(), value_to_find) !=
        some_list.end();
}

void thr01_write()
{
    std::scoped_lock<std::mutex> guard(some_mutex); // 3
    for (auto i = 0; i < 3; ++i)
    {
        add_to_list(i);
    }
}
void thr02_read()
{
    std::scoped_lock<std::mutex> guard(some_mutex); // 4
    for (auto i = 0; i < 3; ++i)
    {
        auto flag = list_contains(i);
        flag_list.push_back(flag);
    }
}
void testMain()
{
    std::cout << "thread::mutex::lockGuard::testMain() begin.\n";
    std::thread thr01(thr01_write);
    std::thread thr02(thr02_read);

    thr01.join();
    thr02.join();

    for (auto& v : flag_list)
    {
        std::cout << "value: "<< v <<".\n";
    }
    std::cout << "thread::mutex::lockGuard::testMain() end.\n";
}
} // namespace lockGuard
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::mutex::testMain() begin.\n";
    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //lockGuard::testMain();
    call_once_demo::testMain();
    std::cout << "thread::mutex::testMain() end.\n";
}
} // namespace mutex
} // namespace thread