#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <cassert>
#include <vector>
using namespace std::literals;
// some memory_order tests
namespace thread::atomic2
{
namespace atomic2_test04
{
std::atomic<int> cnt0 = {0};
std::atomic<int> cnt1 = {0};
int              value = 0;
/*
void f02()
{
    value += 1;
    cnt.store(1, std::memory_order_release);
}
void f01()
{
    while (cnt.load(std::memory_order_acquire) < 1)
    {
    }
    value += 1;
    cnt.store(0, std::memory_order_release);
}
//*/
///*
void f02()
{
    cnt0.store(1, std::memory_order_relaxed);
    cnt1.store(1, std::memory_order_relaxed);
    std::this_thread::sleep_for(15ms);
    value += 1;
}
void f01()
{
    auto v = cnt1.load(std::memory_order_relaxed);
    value += 1;
    v = cnt0.load(std::memory_order_relaxed);
    std::this_thread::sleep_for(15ms);
    //while (cnt.load(std::memory_order_relaxed) < 1)
    //{
    //}
    //value += 1;
    //cnt.store(0, std::memory_order_relaxed);
}
//*/

void testMain()
{
    //std::shared_ptr<int>     p0;
    std::vector<std::thread> v;
    for (int n = 0; n < 5; ++n)
    {
        v.emplace_back(f01);
        v.emplace_back(f02);
    }
    for (auto& t : v)
    {
        t.join();
    }
    std::cout << "Final cnt0 value is " << cnt0 << '\n';
    std::cout << "Final cnt1 value is " << cnt1 << '\n';
    std::cout << "Final value is " << value << '\n';
}
} // namespace atomic2_test03
namespace atomic2_test03
{
std::atomic<int> cnt = {0};

void f()
{
    for (int n = 0; n < 1000; ++n)
    {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}

void testMain()
{
    //std::shared_ptr<int>     p0;
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(f);
    }
    for (auto& t : v)
    {
        t.join();
    }
    std::cout << "Final counter value is " << cnt << '\n';
}
} // namespace a03
namespace atomic2_test02
{
std::atomic<int> x, y;

int r1 = 0;
int r2 = 0;

void func01()
{
    r1 = x.load(std::memory_order_relaxed);
    if (r1 == 42) y.store(r1, std::memory_order_relaxed);
}
void func02()
{
    r2 = y.load(std::memory_order_relaxed);
    if (r2 == 42) x.store(42, std::memory_order_relaxed);
}
void testMain()
{
    std::cout << "atomic2_test02::testMain() begin ..." << std::endl;

    x = 0;
    y = 0;

    std::thread a(func01);
    std::thread b(func02);

    a.join();
    b.join();

    std::cout << "r1: " << r1 << std::endl;
    std::cout << "r2: " << r2 << std::endl;
    std::cout << "atomic2_test02::testMain() end ..." << std::endl;
}
} // namespace atomic2_test02

namespace atomic2_test01_a
{

std::atomic<int> xa, ya;

int r1 = 0;
int r2 = 0;
int r3 = 0;

void f01()
{
    //std::this_thread::sleep_for(15ms);
    while ((r1 = ya.load(std::memory_order_acquire)) != 42)
    {
        std::cout << ".." << std::endl;
        std::this_thread::yield();
    }
    if (r3 == 43) {
        r3 = 52;
    }
    xa.store(r1, std::memory_order_relaxed);
}
void f02()
{
    r3 = 43;
    //std::this_thread::sleep_for(15ms);
    r2 = xa.load(std::memory_order_relaxed);
    ya.store(42, std::memory_order_release);
}
void testMain()
{
    std::cout << "atomic2_test01_a::testMain() begin ..." << std::endl;

    xa = 0;
    ya = 0;

    std::thread a(f01);
    std::thread b(f02);

    a.join();
    b.join();

    std::cout << "r1: " << r1 << std::endl;
    std::cout << "r2: " << r2 << std::endl;
    std::cout << "r3: " << r3 << std::endl;
    std::cout << "atomic2_test01_a::testMain() end ..." << std::endl;
}
} // namespace atomic2_test01
namespace atomic2_test01
{
std::atomic<bool> x, y;
std::atomic<int>  z;
std::atomic<int>  xa, ya;

int r1 = 0;
int r2 = 0;

void write_x_then_y()
{
    r1 = ya.load(std::memory_order_relaxed); // A
    xa.store(r1, std::memory_order_relaxed); // B

    x.store(true, std::memory_order_relaxed); // 1
    y.store(true, std::memory_order_relaxed); // 2
}
void read_y_then_x()
{
    r2 = xa.load(std::memory_order_relaxed); // C
    ya.store(42, std::memory_order_relaxed); // D

    while (!y.load(std::memory_order_relaxed))
        ;                                  // 3
    if (x.load(std::memory_order_relaxed)) // 4
        ++z;
}

template <typename TIterator>
void listSort(TIterator t0, TIterator t1)
{
    if (t0 != t1)
    {

        auto value = *std::next(t0, std::distance(t0, t1) / 2);
        auto ls0   = std::partition(t0, t1, [value](const auto& v) {
            return v < value;
        });
        auto ls1   = std::partition(ls0, t1, [value](const auto& v) {
            return v <= value;
        });
        listSort(t0, ls0);
        listSort(ls1, t1);
    }
}
void testMain()
{
    std::cout << "atomic2_test01::testMain() begin ..." << std::endl;

    x = false;
    y = false;
    z = 0;

    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);

    a.join();
    b.join();

    auto v = z.load();
    assert(v != 0); // 5

    std::cout << "v: " << v << std::endl;
    std::cout << "r1: " << r1 << std::endl;
    std::cout << "r2: " << r2 << std::endl;
    std::cout << "----------------- ------------------------ ---------------------" << std::endl;
    std::vector<int> list{0, -1, 3, 10, -40, 50, 30, 7, 5, 90, 5};
    list.emplace_back(1);
    std::cout << "list old: ";
    for (auto v : list)
    {
        std::cout << v << ",";
    }
    std::cout << std::endl;
    listSort(list.begin(), list.end());
    std::cout << "list new: ";
    for (auto v : list)
    {
        std::cout << v << ",";
    }
    std::cout << std::endl;

    std::cout << "atomic2_test01::testMain() end ..." << std::endl;
}
} // namespace atomic2_test01
void testMain()
{
    std::cout << "thread::atomic2::testMain() begin ..." << std::endl;
    //atomic2_test01::testMain();
    atomic2_test01_a::testMain();
    //atomic2_test02::testMain();
    //atomic2_test03::testMain();
    //atomic2_test04::testMain();
    std::cout << "thread::atomic2::testMain() end ..." << std::endl;
}
} // namespace thread::atomic2