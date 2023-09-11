#include <iostream>
#include <algorithm>
#include <variant>
#include <filesystem>

#include "thread/testAtomic.h"
#include "thread/testAtomic2.h"
#include "thread/testMutex.h"
#include "thread/testSyncConcurrent.h"
#include "thread/testAsync.h"
#include "thread/testThrFuture.h"
#include "thread/testMemoryOrder.h"
#include "thread/testMemoryFence.h"
#include "base/testConstexpr.h"
#include "base/testForward.h"
#include "base/testTypeInfo.h"
#include "base/testString.h"
#include "base/testClass.h"
#include "base/testMacro.h"
#include "base/testMemory.h"
#include "msvcAsm/testInlineAsm.h"
#include "parallel/testExecutionPar.h"
#include "parallel/futureFastSort01.h"
#include "parallel/conditionAndLock.h"
#include "parallel/baseNonLock.h"
#include "parallel/baseNonLock2.h"
#include "parallel/threadPool.h"
#include "parallel/hardware.h"
#include "coding/testCodecvt.h"
#include "demoTemplate/testTemplate.h"
#include "demoTemplate/testTemplate02.h"
#include "exception1/testExcptBase.h"
#include "algorithm/testsort.h"
#include "algorithm/testhash.h"

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

int main01()
{
    std::cout << "run main01 function() ...\n";
    return EXIT_SUCCESS;
}
int main()
{
    exception1::excptBase::testMain();
    return EXIT_SUCCESS;
    demoTemplate::base::testMain();
    return EXIT_SUCCESS;
    algorithm::testhash::testMain();
    return EXIT_SUCCESS;
    algorithm::testsort::testMain();
    return EXIT_SUCCESS;
    demoTemplate::base02::testMain();
    return EXIT_SUCCESS;
    parallel::futureFastSort01::testMain();
    return EXIT_SUCCESS;
    parallel::threadPool::testMain();
    return EXIT_SUCCESS;
    parallel::hardware::testMain();
    return EXIT_SUCCESS;
    base::demoClass::testMain();
    return EXIT_SUCCESS;
    parallel::baseNonLock2::testMain();
    return EXIT_SUCCESS;
    base::testString::testMain();
    return EXIT_SUCCESS;
    parallel::baseNonLock::testMain();
    return EXIT_SUCCESS;
    thread::atomic::testMain();
    return EXIT_SUCCESS;
    parallel::conditionlock::testMain();
    return EXIT_SUCCESS;
    thread::memoryOrder::testMain();
    return EXIT_SUCCESS;
    coding::tcodecvr::testMain();
    return EXIT_SUCCESS;
    thread::memoryFence::testMain();
    return EXIT_SUCCESS;
    thread::atomic2::testMain();
    return EXIT_SUCCESS;
    base::testMemory::testMain();
    return EXIT_SUCCESS;
    /*
    auto path = std::filesystem::path("./");
    path      = std::filesystem::current_path();
    std::cout << "current_path: \n" << path << "\n\n";

    if (const char* env_p = std::getenv("PATH"))
        std::cout << "System PATH is: " << env_p << "\n\n";
    //*/

    base::demoForward::testMain();
    return EXIT_SUCCESS;
    thread::thrFuture::testMain();
    return EXIT_SUCCESS;
    thread::syncConcurrent::testMain();
    return EXIT_SUCCESS;

    //baseMain();
    // thanks: https://learn.microsoft.com/zh-cn/cpp/build/reference/zc-conformance?view=msvc-170
    // thanks: https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170&viewFallbackFrom=vs-2022
    //std::cout << "_MSC_VER : " << _MSC_VER << "\n";
    //std::cout << "_MSVC_LANG  : " << _MSVC_LANG << "\n";

    //parallel::execParallel::testMain();
    //return 1;

    //base::demoConstexpr::testMain();

    msvcAsm::testInline::testMain();
    return EXIT_SUCCESS;

    base::demoMacro::testMain();
    return EXIT_SUCCESS;
    thread::mutex::testMain();
    return EXIT_SUCCESS;

    base::typeInfo::testMain();
    std::cout << "\n";
    return EXIT_SUCCESS;

    std::cout << "\n";
    thread::asyncDemo::testMain();
    return EXIT_SUCCESS;
}
