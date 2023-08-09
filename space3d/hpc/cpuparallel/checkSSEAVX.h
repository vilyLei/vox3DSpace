

/*
	Check SSE/AVX support.
	This application can detect the instruction support of
	SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2, SSE4a, SSE5, and AVX.
*/
// thanks: https://gist.github.com/hi2p-perim/7855506

#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <random>
#include <chrono>
#ifdef _MSC_VER
#include <intrin.h>
// function meaning: value = sqrt(a*a + b*b)
void normal_sqrt(float data1[], float data2[], int len, float out[])
{
    int i;
    for (i = 0; i < len; i++)
    {
        out[i] = sqrt(data1[i] * data1[i] + data2[i] * data2[i]);
    }
}
void simd_sqrt(float* data1, float* data2, int len, float out[])
{
    assert(len % 4 == 0);
    __m128 *a, *b, *res, t1, t2, t3; // = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
    int     i, tlen = len / 4;

    a   = (__m128*)data1;
    b   = (__m128*)data2;
    res = (__m128*)out;
    for (i = 0; i < tlen; i++)
    {

        t1   = _mm_mul_ps(*a, *a);
        t2   = _mm_mul_ps(*b, *b);
        t3   = _mm_add_ps(t1, t2);
        *res = _mm_sqrt_ps(t3);
        a++;
        b++;
        res++;
    }
}
void simd256_sqrt(float* data1, float* data2, int len, float out[])
{
    assert(len % 8 == 0);
    // AVX
    __m256 *a, *b, *res, t1, t2, t3; // = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
    int     i, tlen = len / 8;

    a   = (__m256*)data1;
    b   = (__m256*)data2;
    res = (__m256*)out;
    for (i = 0; i < tlen; i++)
    {

        t1   = _mm256_mul_ps(*a, *a);
        t2   = _mm256_mul_ps(*b, *b);
        t3   = _mm256_add_ps(t1, t2);
        *res = _mm256_sqrt_ps(t3);
        a++;
        b++;
        res++;
    }
}
void test_sqrt()
{
    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_real_distribution<float> distribute(100.5f, 20001.5f);
    constexpr int data_size = 8192;
    float           *data1 = new float[data_size]{};
    float           *data2 = new float[data_size]{};
    float           *data_out = new float[data_size]{};
    std::cout << "data_size: " << data_size << std::endl;
    std::cout << "data: ";
    for (int i = 0; i < data_size; ++i)
    {
        auto v   = distribute(gen);
        data1[i] = v;
        if (i < 8)
            std::cout << distribute(gen) << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < data_size; ++i)
    {
        auto v   = distribute(gen);
        data2[i] = v;
    }

    auto time_start = std::chrono::high_resolution_clock::now();
    //normal_sqrt(data1, data2, data_size, data_out);// 146ms, data_size = 8192 << 12;
    //simd_sqrt(data1, data2, data_size, data_out);// 38ms
    simd256_sqrt(data1, data2, data_size, data_out); //24ms
    // 目前的代码cpu cache miss比较多，优化一下，应该效率更高
    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "loss time: " << lossTime << "ms" << std::endl;

    std::cout << "data_out: ";
    for (int i = 0; i < 8; ++i)
    {
        std::cout << data_out[i] << " ";
    }
    std::cout << std::endl;
}

void __cpuidSpec(int p0[4], int p1)
{
    __cpuid(p0, p1);
}
unsigned __int64 __cdecl _xgetbvSpec(unsigned int p)
{
    return _xgetbv(p);
}

//以上函数 thanks: https://blog.csdn.net/z736248591/article/details/110225352
#endif

#ifdef __GNUC__
#include <xmmintrin.h>
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

#endif
namespace checkcrs
{
template<typename T>
class V4
{
public:
    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        T vs[4];
    };
    T& operator[](int i)
    {
        return vs[i];
    }
};
void testSIMD()
{
    std::cout << "... testSIMD() ..."<< std::endl;
    ///*
    V4<float> v4_0{};
    v4_0.x = 10.0f;
    v4_0.y = 8.5f;
    v4_0[0] += 0.7f;
    v4_0[1] += 0.7f;

    std::cout << " v4_0.x:" << v4_0.x << std::endl;
    std::cout << " v4_0.y:" << v4_0.y << std::endl;
    std::cout << " v4_0.vs[0]:" << v4_0.vs[0] << std::endl;
    std::cout << " v4_0.vs[1]:" << v4_0.vs[1] << std::endl;

    //std::vector<float> vv(4);
    union
    {
        __m128 v;
        //float  vs[4]{4.0f, 4.1f, 4.2f, 4.3f};
        float  vs[4]{4.0f, 4.1f, 4.2f, 4.3f};
        std::array<float,4> array;
    } SIMD4Data{};
    __m128 a4 = _mm_set_ps(4.0f, 4.1f, 4.2f, 4.3f);
    __m128 b4 = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
    __m128 sum4 = _mm_add_ps(a4, b4);
    std::cout << "SSE sum4.m128_f32 A:" << sum4.m128_f32[0] << "," << sum4.m128_f32[1] << "," << sum4.m128_f32[2] << "," << sum4.m128_f32[2] << std::endl;
    std::cout << "SSE sum4.m128_f32 B:" << sum4.m128_f32[3] << "," << sum4.m128_f32[2] << "," << sum4.m128_f32[1] << "," << sum4.m128_f32[0] << std::endl;
    std::cout << "SSE sum4.m128_f32[0]:" << sum4.m128_f32[0] << std::endl;
    SIMD4Data.v = _mm_add_ps(a4, b4);
    std::cout << "SSE Sum SIMD4Data.vs:" << SIMD4Data.vs[0] << "," << SIMD4Data.vs[1] << "," << SIMD4Data.vs[2] << "," << SIMD4Data.vs[3] << std::endl;
    std::cout << "SSE Sum SIMD4Data.array:" << SIMD4Data.array[0] << "," << SIMD4Data.array[1] << "," << SIMD4Data.array[2] << "," << SIMD4Data.array[3] << std::endl;
    float result_vs[4]{};
    _mm_store_ps(result_vs, sum4);
    std::cout << "SSE Sum result_vs:" << result_vs[0] << "," << result_vs[1] << "," << result_vs[2] << "," << result_vs[3] << std::endl;
    std::cout << "\n";
    union
    {
        __m256 v;
        float vs[8]{};
    } SIMD8Data{};

    __m256 a8   = _mm256_set_ps(4.0f, 4.1f, 4.2f, 4.3f, 4.0f, 4.1f, 4.2f, 4.3f);
    __m256 b8   = _mm256_set_ps(3.0f, 3.3f, 3.2f, 3.3f, 3.0f, 3.1f, 3.2f, 3.3f);
    __m256 sum8 = _mm256_add_ps(a8, b8);
    SIMD8Data.v = _mm256_add_ps(a8, b8);
    std::cout << "SSE sum8.m256_f32:" << sum8.m256_f32[0] << "," << sum8.m256_f32[1] << "," << sum8.m256_f32[2] << std::endl;
    std::memcpy(SIMD8Data.vs, sum8.m256_f32, sizeof(float));
    std::cout << "SSE SIMD8Data.vs:" << SIMD8Data.vs[0] << "," << SIMD8Data.vs[1] << "," << SIMD8Data.vs[2] << std::endl;
    std::cout << "\n---------------------------------- -- ----------------\n";
    test_sqrt();
    std::cout << "\n---------------------------------- -- ----------------\n";
    //*/
}
int testMain()
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

    // ----------------------------------------------------------------------

    // Check AVX support
    // References
    // http://software.intel.com/en-us/blogs/2011/04/14/is-avx-enabled/
    // http://insufficientlycomplicated.wordpress.com/2011/11/07/detecting-intel-advanced-vector-extensions-avx-in-visual-studio/

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

    std::boolalpha(std::cout);
    std::cout << "   Support SSE:   " << sseSupportted << std::endl;
    std::cout << "  Support SSE2:   " << sse2Supportted << std::endl;
    std::cout << "  Support SSE3:   " << sse3Supportted << std::endl;
    std::cout << "Support SSE4.1:   " << sse4_1Supportted << std::endl;
    std::cout << "Support SSE4.2:   " << sse4_2Supportted << std::endl;
    std::cout << " Support SSE4a:   " << sse4aSupportted << std::endl;
    std::cout << "  Support SSE5:   " << sse5Supportted << std::endl;
    std::cout << "   Support AVX:   " << avxSupportted << std::endl;

    testSIMD();
    return EXIT_SUCCESS;
}
} // namespace checkcrs
//int main(int argc, char** argv)
//{
//    std::cout << "cpuparallel::main() begin.\n";
//    checkcrs::testMain();
//    std::cout << "cpuparallel::main() end.\n";
//    return EXIT_SUCCESS;
//}