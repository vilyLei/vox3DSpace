#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>
#include <list>
#include <forward_list>
#include <mutex>
#include <algorithm>

namespace demoTemplate::base02
{
namespace templateDemo_b01
{
template <class T>
struct ClassA
{
    struct B
    {};
    T  value;
    typedef void C;
    //typedef void U;
    void         f();

    template <class U>
    void func(U p);
};

template <class B>
void ClassA<B>::f()
{
    B b; // ClassA 的 B ，不是模板形参
}

template <class B>
template <class CT>
void ClassA<B>::func(CT p)
{
    B b; // ClassA 的 B ，不是模板形参
    CT c{}; // 模板形参 C ，不是 A 的 C
    //CT c; // runtime error, 模板形参 C ，不是 A 的 C
    std::cout << "ClassA<B>::func(), p: " << p << std::endl;
    std::cout << "ClassA<B>::func(), c: " << c << std::endl;
    std::cout << "ClassA<B>::func(), value: " << value << std::endl;
}

namespace NS_N
{
class C
{};

template <class T>
class ClassB
{
    void func(T);
};
} // namespace N

template <class C>
void NS_N::ClassB<C>::func(C)
{
    C b; // C 是模板形参，不是 N::C
}

template <auto...>
struct CP
{};

CP<'C', 0, 2L, nullptr> x; // OK

void testMain()
{
    std::forward_list<int> ls001{12,13,14,15};

    auto it = std::next(ls001.begin(), 1);

    ClassA<int> ca{};
    ca.value = 3;
    ca.func(10.1f);

    ClassA<std::string> cb{};
    cb.value = std::string("name");
    cb.func(std::string("hhhh"));

    ClassA<float> cc{};
    cc.value = 15.1f;
    cc.func(std::string("vily"));

}
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "demoTemplate::base02::testMain() begin.\n";
    templateDemo_b01::testMain();
    std::cout << "demoTemplate::base02::testMain() end.\n";

}
} // namespace demoTemplate::base