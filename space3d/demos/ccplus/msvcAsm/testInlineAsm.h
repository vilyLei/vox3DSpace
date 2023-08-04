#include <cstdio>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace msvcAsm
{
namespace testInline
{
namespace test_1
{
// thanks:
// https://learn.microsoft.com/en-us/cpp/assembler/inline/accessing-c-or-cpp-data-in-asm-blocks?view=msvc-170
// https://learn.microsoft.com/en-us/cpp/assembler/inline/asm?view=msvc-170
// inline asm: https://learn.microsoft.com/en-us/cpp/assembler/inline/inline-assembler?view=msvc-170&source=recommendations
struct first_type
{
    char* weasel;
    int   same_name;
};

struct second_type
{
    int  wonton;
    long same_name;
};

int asmMain()
{
    struct first_type hal
    {};
    struct second_type oat
    {};
    // win32 x86平台才能正常运行此 inline asm program
#if _WIN32 && defined(_M_IX86)
    __asm
    {
      lea ebx, hal
      mov ecx, [ebx]hal.same_name ; Must use 'hal'
      mov esi, [ebx].weasel       ; Can omit 'hal'
    }
    ;
#else
    std::cout << "error: msvc inline asm only can run in x86 system !!!\n";
#endif
    return 0;
}
void testMain()
{
    std::cout << "msvcAsm::testInline::test_1::testMain() begin.\n";
    asmMain();
    std::cout << "msvcAsm::testInline::test_1::testMain() end.\n";
}
} // namespace test_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "msvcAsm::testInline::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    test_1::testMain();
    std::cout << "msvcAsm::testInline::testMain() end.\n";
}
} // namespace testInline
} // namespace msvcAsm