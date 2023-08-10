#include <iostream>
#ifdef _MSC_VER
#    include <intrin.h>
void __cpuidSpec(int p0[4], int p1)
{
    __cpuid(p0, p1);
}
unsigned __int64 __cdecl _xgetbvSpec(unsigned int p)
{
    return _xgetbv(p);
}
#endif

#ifdef __GNUC__
void __cpuidSpec(int* cpuinfo, int info)
{
    __asm__ __volatile__(
        "xchg %%ebx, %%edi;"
        "cpuid;"
        "xchg %%ebx, %%edi;"
        : "=a"(cpuinfo[0]), "=D"(cpuinfo[1]), "=c"(cpuinfo[2]), "=d"(cpuinfo[3])
        : "0"(info));
}
unsigned long long _xgetbvSpec(unsigned int index)
{
    unsigned int eax, edx;
    __asm__ __volatile__(
        "xgetbv;"
        : "=a"(eax), "=d"(edx)
        : "c"(index));
    return ((unsigned long long)edx << 32) | eax;
}
#    include <immintrin.h>
#endif

namespace sseavx::config
{
void sseavxCheck()
{
    bool sseSupportted    = false;
    bool sse2Supportted   = false;
    bool sse3Supportted   = false;
    bool ssse3Supportted  = false;
    bool sse4_1Supportted = false;
    bool sse4_2Supportted = false;
    bool sse4aSupportted  = false;
    bool sse5Supportted   = false;
    bool avxSupportted    = false;

    int cpuinfo[4];
    __cpuidSpec(cpuinfo, 1);

    // Check SSE, SSE2, SSE3, SSSE3, SSE4.1, and SSE4.2 support
    sseSupportted    = cpuinfo[3] & (1 << 25) || false;
    sse2Supportted   = cpuinfo[3] & (1 << 26) || false;
    sse3Supportted   = cpuinfo[2] & (1 << 0) || false;
    ssse3Supportted  = cpuinfo[2] & (1 << 9) || false;
    sse4_1Supportted = cpuinfo[2] & (1 << 19) || false;
    sse4_2Supportted = cpuinfo[2] & (1 << 20) || false;

    avxSupportted         = cpuinfo[2] & (1 << 28) || false;
    bool osxsaveSupported = cpuinfo[2] & (1 << 27) || false;
    if (osxsaveSupported && avxSupportted)
    {
        // _XCR_XFEATURE_ENABLED_MASK = 0
        unsigned long long xcrFeatureMask = _xgetbvSpec(0);
        avxSupportted                     = (xcrFeatureMask & 0x6) == 0x6;
    }

    // ----------------------------------------------------------------------

    // Check SSE4a and SSE5 support

    // Get the number of valid extended IDs
    __cpuidSpec(cpuinfo, 0x80000000);
    int numExtendedIds = cpuinfo[0];
    if (numExtendedIds >= 0x80000001)
    {
        __cpuidSpec(cpuinfo, 0x80000001);
        sse4aSupportted = cpuinfo[2] & (1 << 6) || false;
        sse5Supportted  = cpuinfo[2] & (1 << 11) || false;
    }

    // ----------------------------------------------------------------------
    std::cout << "\n";
    std::boolalpha(std::cout);
    std::cout << "Support SSE:     " << sseSupportted << std::endl;
    std::cout << "Support SSE2:    " << sse2Supportted << std::endl;
    std::cout << "Support SSE3:    " << sse3Supportted << std::endl;
    std::cout << "Support SSE4.1:  " << sse4_1Supportted << std::endl;
    std::cout << "Support SSE4.2:  " << sse4_2Supportted << std::endl;
    std::cout << "Support SSE4a:   " << sse4aSupportted << std::endl;
    std::cout << "Support SSE5:    " << sse5Supportted << std::endl;
    std::cout << "Support AVX:     " << avxSupportted << std::endl;
    std::cout << "\n";
}
} // namespace sseavx::config