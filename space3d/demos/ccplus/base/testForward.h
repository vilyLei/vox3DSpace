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
// perfect forwarding(����ת��)
namespace demo_2
{

using std::cout;
using std::endl;
template <typename T>
void func(T& param)
{
    cout << "���������ֵ." << endl;
}
template <typename T>
void func(T&& param)
{
    cout << "���������ֵ." << endl;
}
/*
If a variable or parameter is declared to have type T&& for some deduced type T, that variable or parameter is a universal reference.
���һ���������߲���������ΪT&&������T�Ǳ��Ƶ������ͣ�������������߲�������һ�� universal reference.
*/
// universal reference (��������)
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
    cout << "���������ֵ." << endl;
}
// universal references
template <typename T>
void func(T&& param)
{
    cout << "���������ֵ." << endl;
}
// reference collapsing(�����۵�, ԭ��: �����һ����Ϊ��ֵ���ã�����Ϊ��ֵ���á����򣨼�����������ֵ���ã������Ϊ��ֵ����)
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