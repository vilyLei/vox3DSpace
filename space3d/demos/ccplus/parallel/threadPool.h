#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>
#include <vector>
#include <stdexcept>
#include <functional>
#include <compare>
#include <future>
#include <type_traits>
using namespace std::literals;

namespace parallel::threadPool
{

uint64_t getThisThreadIdWithInt()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    uint64_t id = std::stoull(ss.str());
    return id;
}
std::string getThisThreadIdWithString()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}
template <typename T>
class thread_safe_queue
{
private:
    struct node
    {
        std::shared_ptr<T>    data;
        std::unique_ptr<node> next;
    };
    std::mutex            head_mutex;
    std::unique_ptr<node> head;
    std::mutex            tail_mutex;
    node*                 tail;
    node*                 get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }
    std::unique_ptr<node> pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail())
        {
            return nullptr;
        }
        std::unique_ptr<node> old_head = std::move(head);
        head                           = std::move(old_head->next);
        return old_head;
    }

public:
    thread_safe_queue() :
        head(new node), tail(head.get())
    {}
    thread_safe_queue(const thread_safe_queue& other) = delete;
    thread_safe_queue& operator=(const thread_safe_queue&
                                     other)           = delete;
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head = pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(
            std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node>       p(new node);
        node* const                 new_tail = p.get();
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        tail->next = std::move(p);
        tail       = new_tail;
    }
};
class join_threads
{
    std::vector<std::thread>& threads;

public:
    explicit join_threads(std::vector<std::thread>& threads_) :
        threads(threads_)
    {}
    ~join_threads()
    {
        std::cout << "join_threads::destructor(), threads.size(): " << threads.size() << "\n"
                  << std::flush;
        for (unsigned long i = 0; i < threads.size(); ++i)
        {
            if (threads[i].joinable())
            {
                std::cout << "\tjoin thread i: " << i << "\n";
                threads[i].join();
            }
        }
    }
};
namespace threadPool_02
{
class function_wrapper
{
    struct impl_base
    {
        virtual void call() = 0;
        virtual ~impl_base() {}
    };
    std::unique_ptr<impl_base> impl;

    template <typename F>
    struct impl_type : impl_base
    {
        F f;
        impl_type(F&& f_) :
            f(std::move(f_)) {}
        void call() { f(); }
    };

public:
    template <typename F>
    function_wrapper(F&& f) :
        impl(new impl_type<F>(std::move(f)))
    {}
    void operator()() { impl->call(); }
    function_wrapper() = default;
    function_wrapper(function_wrapper&& other) :
        impl(std::move(other.impl))
    {}
    function_wrapper& operator=(function_wrapper&& other)
    {
        impl = std::move(other.impl);
        return *this;
    }
    function_wrapper(const function_wrapper&)            = delete;
    function_wrapper(function_wrapper&)                  = delete;
    function_wrapper& operator=(const function_wrapper&) = delete;
};
class thread_pool
{
    std::atomic_bool                    done;
    std::vector<std::thread>            threads;
    join_threads                        joiner;
    thread_safe_queue<function_wrapper>          work_queue; // 使用 function_wrapper，而非使用 std::function
    void worker_thread()
    {
        while (!done)
        {
            //function_wrapper task;
            //if (work_queue.try_pop(task))
            //{
            //    task();
            //}
            //else
            //{
            //    std::this_thread::yield();
            //}
            auto task = work_queue.try_pop();
            if (task) // 5
            {
                (*task)(); // 6
            }
            else
            {
                std::this_thread::yield(); // 7
            }
        }
    }

public:
    thread_pool(int thread_total) :
        done(false), joiner(threads)
    {
        unsigned const
            thread_count = thread_total > std::thread::hardware_concurrency() ?
            std::thread::hardware_concurrency() :
            thread_total; // 8
        std::cout << "thread_count: " << thread_count << "\n"
                  << std::flush;

        try
        {
            for (unsigned i = 0; i < thread_count; ++i)
            {
                threads.push_back(
                    std::thread(&thread_pool::worker_thread, this)); // 9
            }
        }
        catch (...)
        {
            done = true; // 10
            throw;
        }
    }
    ~thread_pool()
    {
        std::cout << "thread_pool::destructor()\n";
        done = true; // 11
    }
#if __cplusplus >= 202002L
    template <typename FunctionType>
    std::future<typename std::invoke_result<FunctionType()>::type>
    submit(FunctionType f)
    {
        typedef typename std::invoke_result<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type>          res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
#else
    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type
                                          result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type>          res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }
#endif
    // rest as before
};

void func1()
{
    std::string infoStr = "func1(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}
void func2()
{
    std::string infoStr = "func2(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}

void testMain()
{
    std::cout << "\nhreadPool_02::testMain() begin.\n";

    thread_pool tp(2);
    //tp.submit(func1);
    //tp.submit(func2);


    std::this_thread::sleep_for(600ms);
    std::cout << "threadPool_02::testMain() end.\n";
}
}
namespace threadPool_01
{
///*

class thread_pool
{
    std::atomic_bool                         done;
    thread_safe_queue<std::function<void()>> work_queue; // 1
    std::vector<std::thread>                 threads;    // 2
    join_threads                             joiner;     // 3
    void                                     worker_thread()
    {
        std::string infoStr = "worker_thread() begin, thread id: " + std::to_string(getThisThreadIdWithInt()) + ", done: " + std::to_string(done) + "\n";
        std::cout << infoStr; 
        while (!done) // 4
        {
            auto task = work_queue.try_pop();
            if (task) // 5
            {
                (*task)(); // 6
            }
            else
            {
                std::this_thread::yield(); // 7
            }
        }
        infoStr = "worker_thread() end, thread id: " + std::to_string(getThisThreadIdWithInt()) + "," + std::to_string(done) + "\n";
        std::cout << infoStr; 
    }

public:
    thread_pool(int thread_total) :
        done(false), joiner(threads)
    {
        unsigned const
            thread_count = thread_total > std::thread::hardware_concurrency() ?
            std::thread::hardware_concurrency() :
            thread_total; // 8
        std::cout << "thread_count: " << thread_count << "\n"<<std::flush;

        try
        {
            for (unsigned i = 0; i < thread_count; ++i)
            {
                threads.push_back(
                    std::thread(&thread_pool::worker_thread, this)); // 9
            }
        }
        catch (...)
        {
            done = true; // 10
            throw;
        }
    }
    ~thread_pool()
    {
        std::cout << "thread_pool::destructor()\n";
        done = true; // 11
    }
    template <typename FunctionType>
    void submit(FunctionType f)
    {
        work_queue.push(std::function<void()>(f)); // 12
    }
    void submitFunctionIns(std::function<void()>& f)
    {
        work_queue.push(f);
    }
    };
//*/

void func1()
{
    std::string infoStr = "func1(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}
void func2()
{
    std::string infoStr = "func2(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}
void func3()
{
    std::string infoStr = "func3(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}
void func4()
{
    std::string infoStr = "func4(), thread id: " + getThisThreadIdWithString() + "\n";
    std::cout << infoStr;
    std::this_thread::sleep_for(30ms);
}
class PartCalc
{
public:
    std::string name = "";
    void operator()(){

        std::string infoStr = "PartCalc(" + name + "), thread id: " + getThisThreadIdWithString() + "\n";
        std::cout << infoStr;
        std::this_thread::sleep_for(30ms);
    };
    void test()
    {

        std::string infoStr = "PartCalc(" + name + ")::test, thread id: " + getThisThreadIdWithString() + "\n";
        std::cout << infoStr;
        std::this_thread::sleep_for(30ms);
    };
};
void testMain()
{
    std::cout << "\nhreadPool_01::testMain() begin.\n";

    thread_pool tp(2);
    tp.submit(func1);
    tp.submit(func2);
    tp.submit(func3);
    tp.submit(func4);
    //tp.submit(PartCalc());
    PartCalc p0{};
    p0.name = "p000";
    tp.submit(PartCalc{.name = "pp01"});

    std::function<void()> obj_func1 = std::bind(&PartCalc::test, &p0);
    tp.submit(obj_func1);
    //tp.submitFunctionIns(obj_func1);

    std::this_thread::sleep_for(600ms);
    std::cout << "threadPool_01::testMain() end.\n";
}
} // namespace threadPool_01
namespace threadPool_p01
{
using namespace std::literals::chrono_literals;

void f(std::stop_token stop_token, int value)
{
    while (!stop_token.stop_requested())
    {
        std::cout << value++ << ' ' << std::flush;
        std::this_thread::sleep_for(200ms);
    }
    std::cout << std::endl;
}

struct s_partial_ordering
{
    static const s_partial_ordering less;
    static const s_partial_ordering equivalent;
    static const s_partial_ordering greater;
    static const s_partial_ordering unordered;

    _NODISCARD_FRIEND constexpr bool operator==(const s_partial_ordering _Val, decltype(nullptr)) noexcept
    {
        return _Val._Value == 0;
    }



    unsigned char _Value;
};

void test01()
{
    s_partial_ordering sppo;

    double foo = -0.0;
    double bar = 0.0;

    //auto res = foo <=> bar;
    std::partial_ordering res = foo <=> bar;

    std::cout << "\n";
    std::cout << "sizeof(sppo): " << sizeof(sppo) << "\n";
    std::cout << "res._Value: " << std::to_string(res._Value) << "\n\n";

    if (res < 0)
        std::cout << "-0 is less than 0";
    else if (res > 0)
        std::cout << "-0 is greater than 0";
    else if (res == 0)
        std::cout << "-0 and 0 are equal";
    else
        std::cout << "-0 and 0 are unordered";
    std::cout << "\n";
}
void testMain()
{
    test01();
    std::jthread thread(f, 5); // 打印 5 6 7 8... 约 3 秒
    std::this_thread::sleep_for(3s);
    // jthread 的析构函数调用 request_stop() 与 join()
}
} // namespace threadPool_p01
void testMain()
{
    std::cout << "\nparallel::threadPool::testMain() begin.\n";

    std::stringstream ss;
    ss << 3;
    auto str1 = ss.str();
    auto str2 = ss.str();
    ss.flush();
    auto str3 = ss.str();
    ss << "2"<<std::flush;
    auto str4 = ss.str();
    ss.clear();
    auto str5 = ss.str();
    ss.str("");
    auto str6 = ss.str();

    std::cout << "str1: " << str1 << "\n";
    std::cout << "str2: " << str2 << "\n";
    std::cout << "str3: " << str3 << "\n";
    std::cout << "str4: " << str4 << "\n";
    std::cout << "str5: " << str5 << "\n";
    std::cout << "str6: " << str6 << "\n";

    //std::thread::id thread_old_id;
    //std::cout << "thread_old_id: " << thread_old_id << "\n";
    //threadPool_p01::testMain();
    threadPool_01::testMain();
    std::cout << "parallel::threadPool::testMain() end.\n";
}
} // namespace parallel::threadPool