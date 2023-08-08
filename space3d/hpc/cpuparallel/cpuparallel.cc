
// #include "nmmintrin.h" // for SSE4.2
// #include "immintrin.h" // for AVX 
// #ifdef _WIN32
// //  Windows
// #define cpuid(info, x)    __cpuidex(info, x, 0)
// #else
// //  GCC Intrinsics
// #include <cpuid.h>
// void cpuid(int info[4], int InfoType) {
//     __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
// }
// //bool HW_AVX512F;
// #endif

#include <iostream>
#include "checkSSEAVX.h"

int main(int argc, char** argv)
{
    std::cout << "cpuparallel::main() begin.\n";
    checkcrs::testMain();
    std::cout << "cpuparallel::main() end.\n";
    return EXIT_SUCCESS;
}
