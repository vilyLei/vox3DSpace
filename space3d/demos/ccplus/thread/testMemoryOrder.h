#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>

namespace thread
{
namespace memoryOrder
{
namespace memoryOrder_t07
{
std::vector<int> queue_data;
std::atomic<int> count;
void populate_queue()
{
    std::cout << "memoryOrder_t07::populate_queue() call ..."
              << "\n";
    unsigned const number_of_items = 20;
    queue_data.clear();
    for (unsigned i = 0; i < number_of_items; ++i)
    {
        queue_data.push_back(i);
    }
    count.store(number_of_items, std::memory_order_release); // 1 初始化存储

    //std::this_thread::sleep_for(2ms);
    //for (unsigned i = 0; i < 10; ++i)
    //{
    //    queue_data.push_back(i + number_of_items);
    //}
    //count.store(10, std::memory_order_release); // 1 初始化存储
    for (unsigned i = 0; i < 50; ++i)
    {
        std::this_thread::sleep_for(2ms);
        if (count.load(std::memory_order_acquire) <= 0)
        {
            std::cout << "***...\n";
            for (unsigned i = 0; i < 10; ++i)
            {
                queue_data[i] = (i + number_of_items);
                count.store(10, std::memory_order_release);
            }
            break;
        }
        std::this_thread::yield();
    }
}
void consume_queue_items()
{
    std::cout << "memoryOrder_t07::consume_queue_items() call ..."
              << "\n";
    std::vector<int> vs;
    int              times = 50;
    while (true)
    {
        std::this_thread::sleep_for(2ms);
        int item_index;
        if ((item_index = count.fetch_sub(1, std::memory_order_acquire)) <= 0) // 2 一个“读-改-写”操作
        {
            //wait_for_more_items(); // 3 等待更多元素
            times--;
            if (times < 0 && vs.size() > 0)
            {
                break;
            }
            continue;
        }
        //process(queue_data[item_index - 1]); // 4 安全读取queue_data
        vs.emplace_back(queue_data[item_index - 1]);
        std::this_thread::yield();
    }
    std::string str = "vs:";
    for (auto v : vs)
    {
        str += std::to_string(v) + ",";
    }
    str += "\n";
    std::cout << str;
}
void testMain()
{
    std::cout << "memoryOrder_t07::testMain() call begin ..."
              << "\n";
    std::thread a(populate_queue);
    std::thread b(consume_queue_items); //    202 5.3 同步操作和强制排序
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();
    std::cout << "memoryOrder_t07::testMain() call end ..."
              << "\n";
}
} // namespace memoryOrder_t07
// Release-Acquire ordering
namespace memoryOrder_t06b
{
std::atomic<int> data[5];
std::atomic<int> sync(0);
void             thread_1()
{
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    std::cout << "memoryOrder_t06b::thread_1() call end ..."
              << "\n";
    sync.store(true, std::memory_order_release); // 1.设置sync1
}
void thread_2()
{
    auto status = 1;
    while (!sync.compare_exchange_strong(status, 2, std::memory_order_acq_rel))
        ; // 2.直到 sync1 设置后，循环结束
    status = 1;
    std::cout << "memoryOrder_t06b::thread_2() call end ..."
              << "\n";
}
void thread_3()
{
    while (sync.load(std::memory_order_acquire) < 2)
        ; // 4.直到 sync1 设置后，循环结束
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
    std::cout << "memoryOrder_t06b::thread_3() call end ..."
              << "\n";
}
void testMain()
{
    std::cout << "memoryOrder_t06b::testMain() call begin ..."
              << "\n";
    std::thread t3(thread_3);
    std::thread t2(thread_2);
    std::thread t1(thread_1);
    t1.join();
    t2.join();
    t3.join();
    std::cout << "memoryOrder_t06b::testMain() call end ..."
              << "\n";
}
} // namespace memoryOrder_t06b
// Release-Acquire ordering
namespace memoryOrder_t06
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
    std::cout << "memoryOrder_t06::thread_1() call end ..."
              << "\n";
    sync1.store(true, std::memory_order_release); // 1.设置sync1
}
void thread_2()
{
    while (!sync1.load(std::memory_order_acquire))
        ; // 2.直到 sync1 设置后，循环结束
    std::cout << "memoryOrder_t06::thread_2() call end ..."
              << "\n";
    sync2.store(true, std::memory_order_release); // 3.设置sync2
}
void thread_3()
{
    while (!sync2.load(std::memory_order_acquire))
        ; // 4.直到 sync1 设置后，循环结束
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
    std::cout << "memoryOrder_t06::thread_3() call end ..."
              << "\n";
}
void testMain()
{
    std::cout << "memoryOrder_t06::testMain() call begin ..."
              << "\n";
    std::thread t3(thread_3);
    std::thread t2(thread_2);
    std::thread t1(thread_1);
    t1.join();
    t2.join();
    t3.join();
    std::cout << "memoryOrder_t06::testMain() call end ..."
              << "\n";
}
} // namespace memoryOrder_t06
// Relaxed ordering
namespace memoryOrder_t05
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
    //std::this_thread::sleep_for(5ms);
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
    //std::this_thread::sleep_for(5ms);
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
void testMain()
{
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
}
} // namespace memoryOrder_t05
// Release-Consume ordering
namespace memoryOrder_t04
{

std::atomic<std::string*> ptr;
int                       data;

void producer()
{
    std::string* p = new std::string("Hello");
    data           = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_consume)))
        ;
    assert(*p2 == "Hello"); // 绝无出错： *p2 从 ptr 携带依赖
    assert(data == 42);     // 可能也可能不会出错： data 不从 ptr 携带依赖
    std::cout << "memoryOrder_t04() consumer() ... data: " << data << "\n";
}

void testMain()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
} // namespace memoryOrder_t04
// Release-Acquire ordering
namespace memoryOrder_t03
{
std::vector<int> data;
std::atomic<int> flag = {0};

void thread_1()
{
    data.push_back(42);
    flag.store(1, std::memory_order_release);
}

void thread_2()
{
    int expected = 1;
    while (!flag.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
    {
        expected = 1;
    }
    data.push_back(43);
}

void thread_3()
{
    while (flag.load(std::memory_order_acquire) < 2)
        ;
    //assert(data.at(0) == 42); // 决不出错
    assert(data[0] == 42); // 决不出错

    std::cout << "thread_3() call() ... data.size(): " << data.size() << "\n";
    if (data.size() > 1)
    {
        std::cout << "thread_3() data[1]: " << data[1] << "\n";
    }
}

void testMain()
{
    std::thread a(thread_1);
    std::thread b(thread_2);
    std::thread c(thread_3);
    a.join();
    b.join();
    c.join();
}
} // namespace memoryOrder_t03
// Release-Acquire ordering
namespace memoryOrder_t02
{
std::atomic<std::string*> ptr;
int                       data;

void producer()
{
    std::string* p = new std::string("Hello");
    data           = 42;
    ptr.store(p, std::memory_order_release);
}

void consumer()
{
    std::string* p2;
    while (!(p2 = ptr.load(std::memory_order_acquire)))
        ;
    assert(*p2 == "Hello"); // 绝无问题
    assert(data == 42);     // 绝无问题
}

void testMain()
{
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
} // namespace memoryOrder_t02
// Relaxed ordering
namespace memoryOrder_t01
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
} // namespace memoryOrder_t01
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "thread::memoryOrder::testMain(), begin() ...\n";
    std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //memoryOrder_t01::testMain();
    //memoryOrder_t02::testMain();
    //memoryOrder_t03::testMain();
    //memoryOrder_t04::testMain();
    //memoryOrder_t05::testMain();
    //memoryOrder_t06::testMain();
    //memoryOrder_t06b::testMain();
    memoryOrder_t07::testMain();
    std::cout << "thread::memoryOrder::testMain(), end() ...\n";
}
} // namespace memoryOrder
} // namespace thread