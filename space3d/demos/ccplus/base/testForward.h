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
namespace demo_3
{
template <typename T>
class TUnit
{
public:
    T value;
    TUnit(T v) :
        value(v)
    {
        std::cout << "TUnit::constructor(), value: "<< value <<", ...\n";
    }
    TUnit& addBy(TUnit&& u)
    {
        value += u.value;
        std::cout << "TUnit::addBy(), value: " << value << ", ...\n";
        return *this;
    }
    TUnit& subBy(TUnit& u)
    {
        value -= u.value;
        std::cout << "TUnit::subBy(), value: " << value << ", ...\n";
        return *this;
    }
    TUnit& mulBy(const TUnit& u)
    {
        value *= u.value;
        std::cout << "TUnit::mulBy(), value: " << value << ", ...\n";
        return *this;
    }
    TUnit& addSelf()
    {
        //value -= u.value;
        //std::cout << "TUnit::subBy(), value: " << value << ", ...\n";
        // error
        //addBy(*this);
        // ok
        addBy(std::move(*this));
        return *this;
    }
    virtual ~TUnit()
    {
        std::cout << "TUnit::destructor(), value: " << value << ", ...\n";
    }
};

template <typename T>
void addOne(TUnit<T>&& u)
{
    auto value = u.value + static_cast<T>(1);
    std::cout << "addOne(), value: " << value << ", ...\n";
}
template <typename T>
void addTwo(TUnit<T>&& u)
{
    auto value = u.value + static_cast<T>(2);
    std::cout << "addOne(), value: " << value << ", ...\n";
}
int testMain()
{
    TUnit<float> tu0{1.01f};
    TUnit<float> tu1{1.11f};
    TUnit<float> tu2{1.21f};
    //tu0.addBy(std::forward<TUnit<float>>(tu1));
    tu0.addBy(std::move(tu1));
    //
    tu0.addBy(TUnit<float>{1.0});
    // error
    //tu0.subBy(TUnit<float>{1.0});
    tu0.mulBy(TUnit<float>{1.0});
    tu0.addSelf();
    //tu0.addBy(tu1);
    // error
    //addOne(std::forward<>(tu2));

    return 0;
}
} // namespace demo_3
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
    cout << "warp(), perfect forwarding." << endl;
    func(std::forward<T>(param));
    //func(param);
}
int testMain()
{
    int num = 2019;
    warp(num);
    warp(2019);
    return 0;
}
} // namespace demo_2
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
} // namespace demo_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoForward::testMain() begin.\n";
    //demo_1::testMain();
    //demo_2::testMain();
    demo_3::testMain();
    std::cout << "base::demoForward::testMain() end.\n";
}
} // namespace demoForward
} // namespace base