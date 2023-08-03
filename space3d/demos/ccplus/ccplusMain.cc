#include <iostream>
#include <algorithm>
#include <variant>

#include "thread/testAtomic.h"
#include "thread/testMutex.h"
#include "thread/testSyncConcurrent.h"
#include "thread/testAsync.h"
#include "base/testConstexpr.h"
#include "base/testForward.h"
#include "base/testTypeInfo.h"
#include "base/testString.h"
#include "base/testClass.h"
#include "parallel/testExecutionPar.h"

class ValueUnit
{
public:
    std::string unitName = "";
    void setValue(const std::string&                                 puniform_name,
                    const std::variant<bool, int, const std::string, const char *,  float>& value) const
    {
        const int         location     = 1001;
        const std::string uniform_name = puniform_name;

        std::cout << "uniform_name: " << uniform_name << std::endl;

        struct Visitor
        {
            int program  = 0;
            int location = 1;
            Visitor(int program, int location) :
                program(program), location(location) {}

            void operator()(bool value)
            {
                std::cout << "set bool value: " << value << std::endl;
            }
            void operator()(int value)
            {
                std::cout << "set unsigned int value: " << value << std::endl;
            }
            void operator()(float value)
            {
                std::cout << "set float value: " << value << std::endl;
            }
            void operator()(const std::string& value)
            {
                std::cout << "set string value: " << value << std::endl;
            }
            void operator()(const char* value)
            {
                std::cout << "set const char* value: " << value << std::endl;
            }
        };
        int program = 2001;
        std::visit(Visitor{program, location}, value);
    }
};

template <typename T>
void print(const T& x)
{
    std::cout << "print1: " << x << "\n";
}

int testFunctor1()
{
    int arr[5] = {1, 2, 3, 4, 5};
    std::for_each(arr, arr + 5, print<int>);
    return 0;
}
template <typename T>
struct Print
{
    void operator()(const T& x) const
    {
        std::cout << "print2: " << x << "\n";
    }
};
int testFunctor2()
{
    int arr[5] = {1, 2, 3, 4, 5};
    std::for_each(arr, arr + 5, Print<int>{});
    // for_each(arr, arr + 5, [](auto&& x) { std::cout << x << " "; });
    return 0;
}
struct PrintFuncPtrWrapper
{
    typedef void (*Func_Type)(const int&);
    operator Func_Type() const { return &print<int>; }
};


int testFunctor3()
{
    int arr[5] = {1, 2, 3, 4, 5};
    std::for_each(arr, arr + 5, PrintFuncPtrWrapper{});
    return 0;
}
template <typename T>
struct PrintCount
{
    mutable int count = 0;

    void operator()(const T& x) const
    {
        std::cout << "print3 count(" << count << ") : " << x << std::endl;
        count++;
    }
};

int testFunctor4()
{
    int arr[5] = {1, 2, 3, 4, 5};
    std::for_each(arr, arr + 5, PrintCount<int>{});
    return 0;
}
template <typename T, typename Func, typename... Args>
void for_each_Ex(T* begin, T* end, const Func& f, const Args&... args)
{
    while (begin != end) std::invoke(f, args..., *begin++);
}

template <typename T>
struct Print_Exciting
{
    mutable int count = 0;

    void print(const T& x) const
    {
        std::cout << "Print_Exciting " << count << " : " << x << std::endl;
        count++;
    }
};

int testFunctor5()
{
    int arr[5] = {1, 2, 3, 4, 5};
    for_each_Ex(arr, arr + 5, &Print_Exciting<int>::print, Print_Exciting<int>{});
    return 0;
}
template <typename T, typename Func, typename C>
void for_each_More_Ex(T* begin, T* end, Func C::*f, const C& obj)
{
    while (begin != end) (obj.*(f))(*begin++);
}
int testFunctor6()
{
    int arr[5] = {1, 2, 3, 4, 5};
    for_each_More_Ex(arr, arr + 5, &Print_Exciting<int>::print, Print_Exciting<int>{});
    return 0;
}
int baseMain()
{
    std::variant<int, float, std::string> v;
    v = 10;
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    v = 3.14f;
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    v = "test-string";
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    std::cout << "\n--- --- --- --- --- ---\n" << std::endl;

    ValueUnit valueUnit{};
    valueUnit.setValue("info01", 11.3f);
    //valueUnit.setValue("info02", std::string("tex_shd"));
    valueUnit.setValue("info03", "ps_shd");

    //testFunctor2();
    //testFunctor3();
    //testFunctor4();
    //testFunctor5();
    testFunctor6();

    return EXIT_SUCCESS;
}
//struct VPoint
//{
//    double vx, vy, vz;
//    operator std::string() const
//    {
//        std::string s("(vx=" + std::to_string(vx));
//        s += ",vy=" + std::to_string(vy);
//        s += ",vz=" + std::to_string(vz) + ")";
//        return std::move(s);
//    }
//};
//
//void stringTest()
//{
//    VPoint      v0  = {.vx=0.5};
//    std::string str = "str";
//    str.insert(0, "First");
//    std::cout << "str: "<<str << std::endl;
//    std::string vo_str = v0;
//    std::cout << "vo_str: " << vo_str << std::endl;
//
//}

int main()
{
    //base::testString::testMain();
    //base::demoClass::testMain();
    //return 1;
    //baseMain();
    // thanks: https://learn.microsoft.com/zh-cn/cpp/build/reference/zc-conformance?view=msvc-170
    // thanks: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170&viewFallbackFrom=vs-2022
    //std::cout << "_MSC_VER : " << _MSC_VER << "\n";
    //std::cout << "_MSVC_LANG  : " << _MSVC_LANG << "\n";

    //parallel::execParallel::testMain();
    //return 1;

    //base::demoConstexpr::testMain();
    //thread::atomic::testMain();
    thread::mutex::testMain();
    return EXIT_SUCCESS;

    //base::demoForward::testMain();
    base::typeInfo::testMain();
    std::cout << "\n";
    return EXIT_SUCCESS;

    thread::syncConcurrent::testMain();
    std::cout << "\n";
    thread::asyncDemo::testMain();
    return EXIT_SUCCESS;
}
