#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace demoTemplate::base
{
namespace testDemo4
{
template <class T, int size = sizeof(T)>
class ConsumerCount
{
public:
    int countSize;
    T   users[size];
    ConsumerCount() :
        countSize(size)
    {
    }
    void showInfo()
    {
        std::cout << "ConsumerCount::showInfo(),"
                  << "countSize: " << countSize << std::endl;
    }
};
void testMain()
{
    ConsumerCount<float, 8> cc01{};
    cc01.showInfo();
}
} // namespace testDemo4
namespace test_3
{
#include <iostream>
#include <cstring>
#include <cmath>
// general version
template <class T>
class Compare
{
public:
    static bool IsEqual(const T& lh, const T& rh)
    {
        std::cout << "in the general class..." << std::endl;
        return lh == rh;
    }
};



// specialize for float
template <>
class Compare<float>
{
public:
    static bool IsEqual(const float& lh, const float& rh)
    {
        std::cout << "in the float special class..." << std::endl;

        return std::abs(lh - rh) < 10e-3;
    }
};
// specialize for vector<T>
template <class T>
class Compare<std::vector<T>>
{
public:
    static bool IsEqual(const std::vector<T>& lh, const std::vector<T>& rh)
    {
        if (lh.size() != rh.size()) return false;
        else
        {
            for (int i = 0; i < lh.size(); ++i)
            {
                if (lh[i] != rh[i]) return false;
            }
        }
        return true;
    }
};
// specialize for double
template <>
class Compare<double>
{
public:
    static bool IsEqual(const double& lh, const double& rh)
    {
        std::cout << "in the double special class..." << std::endl;

        return std::abs(lh - rh) < 10e-6;
    }
};
// specialize for any template class type
template <class T1>
struct SpecializedType
{
    T1 x1;
    T1 x2;
};
template <class T>
class Compare<SpecializedType<T>>
{
public:
    static bool IsEqual(const SpecializedType<T>& lh, const SpecializedType<T>& rh)
    {
        return Compare<T>::IsEqual(lh.x1 + lh.x2, rh.x1 + rh.x2);
    }
};
int testMain(void)
{
    std::cout << "demoTemplate::base::test_3::testMain() begin.\n";
    Compare<int> comp1;
    std::cout << comp1.IsEqual(3, 4) << std::endl;
    std::cout << comp1.IsEqual(3, 3) << std::endl;

    Compare<float> comp2;
    std::cout << comp2.IsEqual(3.14, 4.14) << std::endl;
    std::cout << comp2.IsEqual(3, 3) << std::endl;

    Compare<double> comp3;
    std::cout << comp3.IsEqual(3.14159, 4.14159) << std::endl;
    std::cout << comp3.IsEqual(3.14159, 3.14159) << std::endl;

    std::vector<int>          int_vs_0 = {1, 3, 4, 5};
    std::vector<int>          int_vs_1 = {1, 3, 4, 5};
    Compare<std::vector<int>> comp4;
    std::cout << comp4.IsEqual(int_vs_0, int_vs_1) << std::endl;


    std::vector<std::string>          str_vs_0 = {"a", "bbg", "fg"};
    std::vector<std::string>          str_vs_1 = {"a", "bbg", "f9g"};
    Compare<std::vector<std::string>> comp5;
    std::cout << "comp5.IsEqual(str_vs_0, str_vs_1): " << comp5.IsEqual(str_vs_0, str_vs_1) << std::endl;
    std::cout << "demoTemplate::base::test_3::testMain() end.\n";
    return 0;
}
} // namespace test_3
namespace test_2
{
#include <iostream>
#include <cstring>

///  模版特化
template <class T>
int compare(const T left, const T right)
{
    std::cout << "in template<class T>..." << std::endl;
    return (left - right);
}


//  这个是一个特化的函数模版
template <>
int compare<const char*>(const char* left, const char* right)
{
    std::cout << "in special template< >..." << std::endl;

    return strcmp(left, right);
}
//  特化的函数模版, 两个特化的模版本质相同, 因此编译器会报错
// error: redefinition of 'int compare(T, T) [with T = const char*]'|
//template < >
//int compare(const char* left, const char* right)
//{
//    std::cout <<"in special template< >..." <<std::endl;
//
//    return strcmp(left, right);
//}
//  这个其实本质是函数重载
int compare(char* left, char* right)
{
    std::cout << "in char* overload function..." << std::endl;

    return strcmp(left, right);
}
int compare(const std::string& s0, const std::string& s1)
{
    std::cout << "in std::string& overload function..." << std::endl;

    return s0.compare(s1);
}
//函数模板
template <class T>
bool IsEqual(T t1, T t2)
{
    return t1 == t2;
}

template <> //函数模板特化
bool IsEqual(char* t1, char* t2)
{
    return strcmp(t1, t2) == 0;
}

int testMain()
{
    //std::cout << std::showpos;
    compare(1, 4);

    const char* left  = "gatieme";
    const char* right = "jeancheng";
    compare(left, right);

    compare(std::string("str_01"), std::string("str_01"));

    char str1[] = "abc";
    char str2[] = "abc";
    std::cout << std::showpos << "函数模板和函数模板特化" << std::endl;
    std::cout << std::showpos << "IsEqual(1, 1): " << IsEqual(1, 1) << std::endl;
    std::cout << std::noshowpos << "IsEqual(str1, str2): " << IsEqual(str1, str2) << std::endl;
    return 0;
}

} // namespace test_2
namespace test_1
{
#include <iostream>
#include <string>

using namespace std;

template <typename T, typename enable_if<is_arithmetic<T>::value, int>::type = 0>
T applyNumber(T x)
{
    cout << "applyNumber x: " << x << endl;
    return x;
}

template <typename T, typename enable_if<is_integral<T>::value, int>::type = 0>
T f(T x)
{
    cout << "param is int" << endl;
    return x * x;
}

template <typename T, typename enable_if<!is_integral<T>::value, int>::type = 0>
T f(T x)
{
    cout << "param is float" << endl;
    return x + x;
}

int testMain()
{
    cout << f(2) << endl;
    cout << f(3.14) << endl;
    cout << applyNumber(3.14f) << endl;
    cout << applyNumber(31) << endl;
    return 0;
}

//第一个模板函数f，如果T的类型是int，也就is_integral<T>::value的返回值是true时，会导致enable_if的实例化中有一个type的别名。也就是如果T是int，那么会启用该模板函数；否则会禁用该模板函数。
//第二个模板函数的使能是当is_integral<T>::value为false时，也就是T不是int的时候

} // namespace test_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "demoTemplate::base::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //test_1::testMain();
    //test_2::testMain();
    //test_3::testMain();
    testDemo4::testMain();
    std::cout << "demoTemplate::base::testMain() end.\n";
}
} // namespace demoTemplate::base