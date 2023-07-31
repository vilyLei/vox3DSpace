#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace base
{
namespace demoForward
{
// perfect forwarding(完美转发)
namespace demo_2
{

using std::cout;
using std::endl;
template <typename T>
void func(T& param)
{
    cout << "传入的是左值." << endl;
}
template <typename T>
void func(T&& param)
{
    cout << "传入的是右值." << endl;
}
/*
If a variable or parameter is declared to have type T&& for some deduced type T, that variable or parameter is a universal reference.
如果一个变量或者参数被声明为T&&，其中T是被推导的类型，那这个变量或者参数就是一个 universal reference.
*/
// universal reference (万能引用)
template <typename T>
void warp(T&& param)
{
    // perfect forwarding
    func(std::forward<T>(param));
}
int testMain()
{
    int num = 2019;
    warp(num);
    warp(2019);
    return 0;
}
}
namespace demo_1
{
using std::cout;
using std::endl;
template <typename T>
void func(T& param)
{
    cout << "传入的是左值." << endl;
}
// universal references
template <typename T>
void func(T&& param)
{
    cout << "传入的是右值." << endl;
}
// reference collapsing(引用折叠, 原则: 如果任一引用为左值引用，则结果为左值引用。否则（即两个都是右值引用），结果为右值引用)
template <typename T>
void warp(T&& param)
{
    func(param);
}
int testMain()
{
    int num = 2019;
    warp(num);
    warp(2019);
    return 0;
}
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoForward::testMain() begin.\n";
    //demo_1::testMain();
    demo_2::testMain();
    std::cout << "base::demoForward::testMain() end.\n";
}
} // namespace demoForward
} // namespace base