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
namespace templateTemplateParameter
{
// ˫��ģ�����

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#    define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
template <typename T, template <class, class...> class C, class... Args>
std::ostream& operator<<(std::ostream& os, const C<T, Args...>& objs)
{
    // ���Խ�����ȫ����ӡ����
    os << "FUNC DETAIL: " << __PRETTY_FUNCTION__ << '\n';
    for (auto const& obj : objs)
        os << obj << ' ';
    return os;
}
template <template <class> class H, class S>
S calcListTotalValue(const H<S>& ls)
{
    auto a = std::begin(ls);
    auto b = std::end(ls);
    S    totalValue{};
    for (auto i = a; i != b; ++i)
    {
        totalValue += *i;
    }
    return totalValue;
}
//template <template <class, class> class V, class T, class A>
template <template <typename, typename> class V, typename T, typename A>
T calcListTotalValue2(V<T, A>& ls)
{
    auto a = std::begin(ls);
    auto b = std::end(ls);
    T    totalValue{};
    for (auto i = a; i != b; ++i)
    {
        totalValue += *i;
    }
    return totalValue;
}
void testMain()
{
    std::cout << "\ntemplateTemplateParameter::testMain() begin.\n";

    std::list<int> list_01{10,3,5};
    auto value1 = calcListTotalValue<std::list, int>(list_01);
    value1 = calcListTotalValue<std::list>(list_01);
    std::cout << "value1: " << value1 << "\n";
    calcListTotalValue2<std::list, int>(list_01);
    auto value2 = calcListTotalValue2(list_01);
    std::cout << "value2: " << value2 << "\n";

    std::cout << "list_01: " << list_01 << "\n";
    std::cout << "templateTemplateParameter::testMain() end.\n";
}
} // namespace templateDefTest03
namespace templateDefTest03
{
template <auto...>
struct ClassA
{};

ClassA<'C', 0, 2L, nullptr> x; // OK

template <typename A, typename B, typename C = std::less<>>
bool func01(A a, B b, C cmp = C{})
{
    //std::cout << "call templateDefTest03::func01() ";
    return cmp(a, b);
}
void testMain()
{
    std::cout
        << std::boolalpha
        << func01(1, 2) << ' '                // true
        << func01(1.0, 1) << ' '              // false
        << func01(1, 2.0) << ' '              // true
        << std::less<int>{}(5, 5.6) << ' '    // false �� 5 < 5 �����棺��ʽת����
        << std::less<double>{}(5, 5.6) << ' ' // true  �� 5.0 < 5.6
        << std::less<int>{}(5.6, 5.7) << ' '  // false �� 5 < 5 �����棺��ʽת����
        << std::less{}(5, 5.6) << ' '         // true  �� less<void>: 5.0 < 5.6
        << '\n';
}
}
namespace templateDefTest02
{
struct ListNode
{
    int a = 10;
    int b = 10;
};
struct Data
{
    int a = 10;
    int b = 10;

};
class T
{
public:
    class U;
    class F;

private:
    int U;
};

template <typename T>
struct Node
{
    struct Node* Next;   // OK���� Node �Ĳ����ҵ�ע�������
    struct Data* Data;   // OK����ȫ���������������� Data
                         // ���������ݳ�Ա Data
    //friend class ::List; // ���󣺲����������޶���
    enum Kind* kind;     // ���󣺲�������ö��
};

Data* p; // OK��struct Data �ѱ�����
void main()
{
    T           t0;
    int         T;
    //T           t; // �����ҵ��ֲ����� T
    class T     t; // OK���ҵ� ::T�����Ծֲ����� T
    T::F*       f; // OK
    //T::U*       u; // ����T::U �Ĳ����ҵ�˽�����ݳ�Ա
    class T::U* u; // OK�����Ը����ݳ�Ա
}
}
namespace templateDefTest01
{
template <typename T>
struct eval; // primary template

template <template <typename, typename...> class TT, typename T1, typename... Rest>
struct eval<TT<T1, Rest...>>
{}; // partial specialization of eval

template <typename T1> struct A;
template <typename T1, typename T2> struct B;
template <int N> struct C;
template <typename T1, int N> struct D;
template <typename T1, typename T2, int N = 17> struct E;

eval<A<int>>        eA; // OK: matches partial specialization of eval
eval<B<int, float>> eB; // OK: matches partial specialization of eval
/*
eval<C<17>>         eC; // error: C does not match TT in partial specialization
// because TT's first parameter is a
// type template parameter, while 17 does not name a type
eval<D<int, 17>> eD; // error: D does not match TT in partial specialization
// because TT's second parameter is a
// type parameter pack, while 17 does not name a type
eval<E<int, float>> eE; // error: E does not match TT in partial specialization
//*/
// because E's third (default) parameter is a non-type
} // namespace templateDefTest

namespace template_test_03
{
// ��������ģ�����(Nontype Class Template Parameters)
template <class T>
void f()
{
    std::cout << "# 1" << std::endl;
} // #1

template <int I>
void f()
{
    std::cout << "# 2, I: " << I << std::endl;
} // #2

void testMain()
{
    f<int()>();
    f<int>();
    f<12>();
}
}
 namespace template_test_02
 {
 template <class T>
 constexpr T pi = T(3.1415926535897932385L); // ����ģ��

 template <class T>
 T circular_area(T r) // ����ģ��
 {
     return pi<T> * r * r; // pi<T> �Ǳ���ģ��ʵ����
 }
 }
namespace buildEnableIf
{
template <bool _Test, class _Ty = void>
struct TEnable_if
{}; // no member "type" when !_Test

template <class _Ty>
struct TEnable_if<true, _Ty>
{ // type is _Ty for _Test
    using type = _Ty;
};
template <bool _Test, class _Ty = void>
using TEnable_if_t = typename TEnable_if<_Test, _Ty>::type;

struct T1
{
    enum
    {
        int_t,
        float_t,
        void_t
    } type;

    template <typename Integer,
              TEnable_if_t<std::is_integral<Integer>::value, bool> = true>
    T1(Integer) :
        type(int_t) {}

    template <typename Floating,
              TEnable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    T1(Floating) :
        type(float_t) {} // OK
        
    template <typename VoidT,
              TEnable_if_t<std::is_void<VoidT>::value, bool> = true>
    T1(VoidT) :
        type(void_t) {} // OK
};
struct T2
{
    enum
    {
        int_t,
        float_t
    } type;

    template <typename Integer,
              TEnable_if_t<std::is_integral<Integer>::value, bool> = true>
    T2(Integer) :
        type(int_t) {
        std::cout << "T2 constructor() A.\n";
    }

    template <typename Floating,
              TEnable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    T2(Floating) :
        type(float_t)
    {
        std::cout << "T2 constructor() B.\n";
    } // OK
};

template <typename T>
typename TEnable_if<std::is_arithmetic<T>::value>::type applyNumber(T value)
{
    std::cout << "TEnable_if, applyNumber valye: " << value << std::endl;
    //return value;
}
template <typename T, typename = TEnable_if<std::is_arithmetic<T>::value>::type>
T calcNumber(T value)
{
    std::cout << "TEnable_if, calcNumber valye: " << value << std::endl;
    return value;
}
template <typename T, typename = TEnable_if<std::is_arithmetic<T>::value>>
T calcNumber2(T value)
{
    std::cout << "TEnable_if, calcNumber valye: " << value << std::endl;
    return value;
}

void testMain()
{
    //std::cout << calcNumber("df") << std::endl;
    std::cout << calcNumber(31) << std::endl;
    std::cout << calcNumber(31.4) << std::endl;
    std::cout << calcNumber2(31) << std::endl;
    std::cout << calcNumber2(31.4) << std::endl;
    applyNumber(56);
    applyNumber(56.0);
    T1 t1_0{1};
    T1 t1_1{11.1};
    //T1 t1_2(void);
    std::cout << "t1_0: " << t1_0.type << std::endl;
    std::cout << "t1_1: " << t1_1.type << std::endl;
    //std::cout << "t1_2: " << t1_2 << std::endl;

    
    T2 t2_0{1};
    T2 t2_1{11.1};
    std::cout << "t2_0: " << t2_0.type << std::endl;
    std::cout << "t2_1: " << t2_1.type << std::endl;
}
} // namespace buildEnableIf
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

///  ģ���ػ�
template <class T>
int compare(const T left, const T right)
{
    std::cout << "in template<class T>..." << std::endl;
    return (left - right);
}


//  �����һ���ػ��ĺ���ģ��
template <>
int compare<const char*>(const char* left, const char* right)
{
    std::cout << "in special template< >..." << std::endl;

    return strcmp(left, right);
}
//  �ػ��ĺ���ģ��, �����ػ���ģ�汾����ͬ, ��˱������ᱨ��
// error: redefinition of 'int compare(T, T) [with T = const char*]'|
//template < >
//int compare(const char* left, const char* right)
//{
//    std::cout <<"in special template< >..." <<std::endl;
//
//    return strcmp(left, right);
//}
//  �����ʵ�����Ǻ�������
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
//����ģ��
template <class T>
bool IsEqual(T t1, T t2)
{
    return t1 == t2;
}

template <> //����ģ���ػ�
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
    std::cout << std::showpos << "����ģ��ͺ���ģ���ػ�" << std::endl;
    std::cout << std::showpos << "IsEqual(1, 1): " << IsEqual(1, 1) << std::endl;
    std::cout << std::noshowpos << "IsEqual(str1, str2): " << IsEqual(str1, str2) << std::endl;
    return 0;
}

} // namespace test_2
namespace test_1
{
using namespace std;

template <class T>
typename std::enable_if<std::is_trivially_default_constructible<T>::value>::type
construct(T*)
{
    std::cout << "default constructing trivially default constructible T\n";
}

template <typename T>
typename enable_if<is_arithmetic<T>::value>::type applyNumberA(T x)
{
    cout << "applyNumberA() x: " << x << endl;
}
template <typename T, typename = enable_if<is_arithmetic<T>::value>>
T applyNumber(T x)
{
    cout << "applyNumber x: " << x << endl;
    return x;
}

template <typename T, typename = enable_if<is_integral<T>::value, int>::type>
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
    applyNumberA(3.14f);
    applyNumberA(31);
    return 0;
}

//��һ��ģ�庯��f�����T��������int��Ҳ��is_integral<T>::value�ķ���ֵ��trueʱ���ᵼ��enable_if��ʵ��������һ��type�ı�����Ҳ�������T��int����ô�����ø�ģ�庯�����������ø�ģ�庯����
//�ڶ���ģ�庯����ʹ���ǵ�is_integral<T>::valueΪfalseʱ��Ҳ����T����int��ʱ��

} // namespace test_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "demoTemplate::base::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //test_1::testMain();
    buildEnableIf::testMain();
    //test_2::testMain();
    //test_3::testMain();
    //testDemo4::testMain();
    template_test_03::testMain();
    templateDefTest03::testMain();
    templateTemplateParameter::testMain();
    std::cout << "demoTemplate::base::testMain() end.\n";
}
} // namespace demoTemplate::base