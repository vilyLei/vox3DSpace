#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>

namespace thread
{
namespace atomic
{
namespace memory_order_relaxed_1
{
std::atomic<int> cnt = {0};
std::atomic<int> x = {0};
std::atomic<int> y = {0};
std::atomic<int> r1 = {0};
std::atomic<int> r2 = {0};

void thr_func_0()
{
    r1 = y.load(std::memory_order_relaxed); // A
    x.store(r1, std::memory_order_relaxed); // B
}
void thr_func_1()
{
    r2 = x.load(std::memory_order_relaxed); // C
    y.store(42, std::memory_order_relaxed); // D
}
    void             f()
{
    for (int n = 0; n < 1000; ++n)
    {
        cnt.fetch_add(1, std::memory_order_relaxed);
    }
}

void test02()
{
    std::cout << "memory_order_relaxed_1::test02() begin ...\n";
    std::vector<std::thread> v;
    v.emplace_back(thr_func_0);
    v.emplace_back(thr_func_1);
    for (auto& t : v)
    {
        t.join();
    }

    std::cout << "x: " << x << "\n";
    std::cout << "y: " << y << "\n";
    std::cout << "r1: " << r1 << "\n";
    std::cout << "r2: " << r2 << "\n";
    std::cout << "memory_order_relaxed_1::test02() end ...\n";
}
    void test01()
{
    std::cout << "memory_order_relaxed_1::test01() begin ...\n";
    auto threadsTotal = std::thread::hardware_concurrency();
    std::cout << "threadsTotal: " << threadsTotal << "\n";
    std::cout << "memory_order_relaxed_1::test01() begin ...\n";
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(f);
    }
    for (auto& t : v)
    {
        t.join();
    }
    assert(cnt == 10000); // never failed
    std::cout << "cnt: " << cnt << "\n";
    std::cout << "memory_order_relaxed_1::test01() end ...\n";
}
int testMain()
{
    //test01();
    test02();
    return 0;
}
} // namespace memory_order_relaxed_1
namespace compare_exchange_strong_01
{
std::atomic<int> atomic_0;
int              tst_val   = 4;
int              new_val   = 5;
bool             exchanged = false;
void             valsout()
{
    std::cout << "atomic_0 = " << atomic_0
              << ",  tst_val = " << tst_val
              << ",  new_val = " << new_val
              << ",  exchanged = " << std::boolalpha << exchanged
              << "\n";
}
int testMain()
{
    std::cout << "compare_exchange_strong::testMain() begin ...\n";
    atomic_0 = 3;
    valsout();
    // tst_val != atomic_0   ==>  tst_val is modified
    exchanged = atomic_0.compare_exchange_strong(tst_val, new_val);
    valsout();
    // tst_val == atomic_0   ==>  atomic_0 is modified
    exchanged = atomic_0.compare_exchange_strong(tst_val, new_val);
    valsout();
    // tst_val == atomic_0   ==>  atomic_0 is modified
    exchanged = atomic_0.compare_exchange_weak(tst_val, new_val);
    valsout();
    std::cout << "compare_exchange_strong::testMain() end ...\n";
    return EXIT_SUCCESS;
}
} // namespace compare_exchange_strong

namespace mem_order_acq_rel3
{
std::atomic<int>  data[5];
std::atomic<bool> sync1(false), sync2(false);
void              thread_1()
{
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync1.store(true, std::memory_order_release); // 1.设置sync1
    std::cout << "thread_1() ...\n";
}
void thread_2()
{
    while (!sync1.load(std::memory_order_acquire))
        ;                                         // 2.直到sync1设置后，循环结束
    sync2.store(true, std::memory_order_release); // 3.设置sync2
    std::cout << "thread_2() ...\n";
}
void thread_3()
{
    while (!sync2.load(std::memory_order_acquire))
        ; // 4.直到sync1设置后，循环结束
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
    std::cout << "thread_3() ...\n";
}
// 保持逻辑同步
int testMain()
{
    std::cout << "mem_order_relaxed3::testMain() begin ...\n";
    std::thread t1(thread_1);
    std::thread t2(thread_2);
    std::thread t3(thread_3);

    //t3.detach();
    t3.join();
    std::cout << "mem_order_relaxed3 01...\n";
    t2.join();
    std::cout << "mem_order_relaxed3 02...\n";
    t1.join();
    std::cout << "mem_order_relaxed3 03...\n";


    std::cout << "mem_order_relaxed3::testMain() end ...\n";
    return EXIT_SUCCESS;
}
} // namespace mem_order_acq_rel3
namespace mem_order_acq_rel2
{
std::atomic<bool> x, y;
std::atomic<int>  z;
void              write_x_then_y()
{
    x.store(true, std::memory_order_relaxed); // 1
    y.store(true, std::memory_order_release); // 2
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_acquire))
        ;                                  // 3 自旋，等待y被设置为true
    if (x.load(std::memory_order_relaxed)) // 4
        ++z;
}
int testMain()
{
    std::cout << "mem_order_acq_rel2::testMain() begin ...\n";
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() != 0); // 5
    std::cout << "mem_order_acq_rel2, z.load() " << z.load() << '\n';
    std::cout << "mem_order_acq_rel2::testMain() end ...\n";
    return 1;
}

} // namespace mem_order_acq_rel2
namespace mem_order_acq_rel
{
std::atomic<bool> x, y;
std::atomic<int>  z;
void              write_x()
{
    x.store(true, std::memory_order_release);
}
void write_y()
{
    y.store(true, std::memory_order_release);
}
void read_x_then_y()
{
    while (!x.load(std::memory_order_acquire))
        ;
    if (y.load(std::memory_order_acquire)) // 1
        ++z;
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_acquire))
        ;
    if (x.load(std::memory_order_acquire)) // 2
        ++z;
}
int testMain()
{
    std::cout << "mem_order_acq_rel::testMain() begin ...\n";
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join();
    b.join();
    c.join();
    d.join();
    assert(z.load() != 0); // 3
    std::cout << "mem_order_acq_rel, z.load() " << z.load() << '\n';
    std::cout << "mem_order_acq_rel::testMain() end ...\n";
    return 1;
}
} // namespace mem_order_acq_rel
namespace mem_order_relaxed2
{
std::atomic<int>  x(0), y(0), z(0); // 1
std::atomic<bool> go(false);        // 2
unsigned const    loop_count = 10;
struct read_values
{
    int x, y, z;
};
read_values values1[loop_count];
read_values values2[loop_count];
read_values values3[loop_count];
read_values values4[loop_count];
read_values values5[loop_count];
void        increment(std::atomic<int>* var_to_inc, read_values* values)
{
    while (!go)
        std::this_thread::yield(); // 3 自旋，等待信号
    for (unsigned i = 0; i < loop_count; ++i)
    {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        var_to_inc->store(i + 1, std::memory_order_relaxed); // 4
        std::this_thread::yield();
    }
}
void read_vals(read_values* values)
{
    while (!go)
        std::this_thread::yield(); // 5 自旋，等待信号
    for (unsigned i = 0; i < loop_count; ++i)
    {
        values[i].x = x.load(std::memory_order_relaxed);
        values[i].y = y.load(std::memory_order_relaxed);
        values[i].z = z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}
void print(read_values* v)
{
    for (unsigned i = 0; i < loop_count; ++i)
    {
        if (i)
            std::cout << ",";
        std::cout << "(" << v[i].x << "," << v[i].y << "," << v[i].z << ")";
    }
    std::cout << std::endl;
}
int testMain()
{
    std::cout << "mem_order_relaxed2::testMain() begin ...\n";
    std::thread t1(increment, &x, values1);
    std::thread t2(increment, &y, values2);
    std::thread t3(increment, &z, values3);
    std::thread t4(read_vals, values4);
    std::thread t5(read_vals, values5);
    go = true; // 6 开始执行主循环的信号
    t5.join();
    t4.join();
    t3.join();
    t2.join();
    t1.join();
    print(values1); // 7 打印最终结果
    print(values2);
    print(values3);
    print(values4);
    print(values5);

    std::cout << "mem_order_relaxed2::testMain() end ...\n";
    return EXIT_SUCCESS;
}
} // namespace mem_order_relaxed2
namespace mem_order_relaxed
{
std::atomic<bool> x, y;
std::atomic<int>  z;
void              write_x_then_y()
{
    x.store(true, std::memory_order_relaxed); // 1
    y.store(true, std::memory_order_relaxed); // 2
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_relaxed))
        ;                                  // 3
    if (x.load(std::memory_order_relaxed)) // 4
        ++z;
}
int testMain()
{
    std::cout << "mem_order_relaxed begin ...\n";
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    // may be have false
    assert(z.load() != 0); // 5

    std::cout << "mem_order_relaxed, z.load() " << z.load() << '\n';
    std::cout << "mem_order_relaxed::testMain() end ...\n";
    return 1;
}
} // namespace mem_order_relaxed
namespace mem_seq_cst
{

std::atomic<bool> x, y;
std::atomic<int>  z;
void              write_x()
{
    x.store(true, std::memory_order_seq_cst); // 1
}
void write_y()
{
    y.store(true, std::memory_order_seq_cst); // 2
}
void read_x_then_y()
{
    while (!x.load(std::memory_order_seq_cst))
        ;
    if (y.load(std::memory_order_seq_cst)) // 3
        ++z;
}
void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst))
        ;
    if (x.load(std::memory_order_seq_cst)) // 4
        ++z;
}
int testMain()
{
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join();
    b.join();
    c.join();
    d.join();
    assert(z.load() != 0); // 5
    std::cout << "mem_seq_cst, z.load() " << z.load() << '\n';
    std::cout << "mem_seq_cst::testMain() end ...\n";
    return 1;
}
} // namespace mem_seq_cst

namespace atomic_bool
{

std::vector<int>  data;
int               data_index;
std::atomic<bool> data_ready(false);

void reader_thread(int id)
{
    while (!data_ready.load()) // 1
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << ">>> reader_thread() id: " << id << "..\n"; // 2
    for (auto& v : data)
    {
        std::cout << "data value:" << v << "\n"; // 2
    }
    data_ready = false;
}
void writer_thread()
{
    while (data_ready.load()) // 5
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    data_index++;
    data.push_back(data_index); // 3
    data_ready = true;          // 4
}

int testMain()
{
    std::vector<std::thread> v;
    for (int i = 0; i < 5; ++i)
    {
        v.emplace_back(reader_thread, i);
    }
    for (int n = 0; n < 5; ++n)
    {
        v.emplace_back(writer_thread);
    }

    for (auto& t : v)
    {
        t.join();
    }
    for (auto& v : data)
    {
        std::cout << ">>> data value:" << v << "\n"; // 2
    }
    std::cout << "thread::atomic::test02().\n";
    return 1;
}

} // namespace atomic_bool


std::atomic_flag flag_lock = ATOMIC_FLAG_INIT;

void func01(int n)
{
    while (flag_lock.test_and_set(std::memory_order_acquire))
    {     // acquire flag_lock
        ; //auto spin
    }
    std::cout << "Output from thread " << n << '\n';
    flag_lock.clear(std::memory_order_release); // release flag_lock
}

int test01()
{
    std::vector<std::thread> v;
    for (int n = 0; n < 10; ++n)
    {
        v.emplace_back(func01, n);
    }
    for (auto& t : v)
    {
        t.join();
    }
    return 1;
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::atomic::testMain().\n";
    std::cout << std::atomic<int>::is_always_lock_free << "\n";

    //test01();
    //atomic_bool::testMain();
    //mem_seq_cst::testMain();
    //mem_order_relaxed::testMain();
    //mem_order_relaxed2::testMain();
    //mem_order_acq_rel::testMain();
    //mem_order_acq_rel2::testMain();
    //mem_order_acq_rel3::testMain();
    compare_exchange_strong_01::testMain();
    //memory_order_relaxed_1::testMain();
}
} // namespace atomic
} // namespace thread