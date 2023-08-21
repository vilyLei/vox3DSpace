#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <queue>
#include <memory>
#include <bitset>

namespace parallel::conditionlock
{
namespace conditionlock_03
{
template <typename T>
class threadsafe_queue
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
    threadsafe_queue() :
        head(new node), tail(head.get())
    {}
    threadsafe_queue(const threadsafe_queue& other) = delete;
    threadsafe_queue&  operator=(const threadsafe_queue&
                                    other)         = delete;
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
} // namespace conditionlock_03
namespace conditionlock_02
{
template <typename T>
class threadsafe_queue
{
private:
    mutable std::mutex             mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable        data_cond;

public:
    threadsafe_queue() {}
    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(*data_queue.front()); // 1
        data_queue.pop();
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = std::move(*data_queue.front()); // 2
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        std::shared_ptr<T> res = data_queue.front(); // 3
        data_queue.pop();
        return res;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res = data_queue.front(); // 4
        data_queue.pop();
        return res;
    }
    void push(T new_value)
    {
        std::shared_ptr<T> data(
            std::make_shared<T>(std::move(new_value))); // 5
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
} // namespace conditionlock_02
namespace conditionlock_01
{
template <typename T>
class threadsafe_queue
{
private:
    mutable std::mutex      mut;
    std::queue<T>           data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue()
    {}
    void push(T new_value)
    {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one(); // 1
    }
    void wait_and_pop(T& value) // 2
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop() // 3
    {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); }); // 4
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = std::move(data_queue.front());
        data_queue.pop();
        return true;
    }
    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>(); // 5
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};
template <class _Integral, class _Ty>
_NODISCARD _Integral _Atomic_reinterpret_as(const _Ty& _Source) noexcept
{
    // interprets _Source as the supplied integral type
    static_assert(is_integral_v<_Integral>, "Tried to reinterpret memory as non-integral");
    if constexpr (is_integral_v<_Ty> && sizeof(_Integral) == sizeof(_Ty))
    {
        return static_cast<_Integral>(_Source);
    }
    else if constexpr (is_pointer_v<_Ty> && sizeof(_Integral) == sizeof(_Ty))
    {
        return reinterpret_cast<_Integral>(_Source);
    }
    else
    {
        _Integral _Result{}; // zero padding bits
        _CSTD     memcpy(&_Result, _STD addressof(_Source), sizeof(_Source));
        return _Result;
    }
}
void testMain()
{
    //std::shared_ptr<int> ptr_res( std::make_shared<int>(std::move(nullptr)));
    //std::shared_ptr<int> ptr_res = nullptr;
    std::shared_ptr<int> ptr_res_0 = std::shared_ptr<int>();
    if (ptr_res_0)
    {
        std::cout << "conditionlock_01::testMain(), ptr_res_0 != nullptr.\n";
    }
    else
    {
        std::cout << "conditionlock_01::testMain(), ptr_res_0 == nullptr.\n";
    }
    std::shared_ptr<int> ptr_res_1 = std::shared_ptr<int>();

    std::cout << "conditionlock_01::testMain(), ptr_res_0 == ptr_res_1: " << (ptr_res_0 == ptr_res_1) << "\n";

    int*                 pInt    = new int(42);
    std::shared_ptr<int> pShared = std::make_shared<int>(42);

    //auto output = [](std::string_view msg, int* const pInt) {
    auto output = [](std::string_view msg, int const* pInt) {
        int pa = 0;
        //pInt   = &pa;
        //*pInt   = 10;
        std::cout << msg << *pInt << " in " << pInt << '\n';
    };

    output("Naked pointer: ", pInt);
    //  output("Shared pointer: ", pShared); // compiler error
    output("Shared pointer: ", &*pShared); // OK, calls operator*, then takes addr
    output("Shared pointer with get(): ", pShared.get());

    std::cout << "sizeof(int): " << sizeof(int) << std::endl;
    std::cout << "sizeof(void*): " << sizeof(void*) << std::endl;
    
    auto _TypeSize = 16;

    std::bitset<8> bit_bin_value_a(_TypeSize);
    std::bitset<8> bit_bin_value_b((_TypeSize - 1));
    //std::cout << std::format("{:b}", bin_value);
    std::cout << "bit_bin_value_a: " << bit_bin_value_a << std::endl;
    std::cout << "bit_bin_value_b: " << bit_bin_value_b << std::endl;
    std::cout << "(_TypeSize & (_TypeSize - 1)): " << (_TypeSize & (_TypeSize - 1)) << std::endl;
    auto flag      = _TypeSize <= 8 && (_TypeSize & (_TypeSize - 1)) == 0;
    std::cout << "flag: " << flag << std::endl;
}

struct Struct_X
{
    int  x, y;
    int  operator()(int);
    void func()
    {
        // the context of the following lambda is the member function X::f
        [=]() -> int {
            return operator()(this->x + y); // X::operator()(this->x + (*this).y)
                                            // this has type X*
        };
    }
};
} // namespace conditionlock_01
void testMain()
{
    std::cout << "parallel::conditionlock::testMain() begin.\n";
    auto t = conditionlock_01::Struct_X(5);
    //x0.func();

    conditionlock_01::testMain();
    std::cout << "parallel::conditionlock::testMain() end.\n";
}
} // namespace parallel::conditionlock