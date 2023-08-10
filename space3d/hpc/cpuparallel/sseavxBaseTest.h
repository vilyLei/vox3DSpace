#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <random>
#include <chrono>

#ifdef _MSC_VER
#    include <intrin.h>
#endif

#ifdef __GNUC__
#include <cstring>
#include <immintrin.h>
#endif

namespace sseavx::test
{
namespace
{
// function meaning: value = sqrt(a*a + b*b)
void normal_sqrt_calc(float data1[], float data2[], int len, float out[])
{
    int i;
    for (i = 0; i < len; i++)
    {
        out[i] = sqrt(data1[i] * data1[i] + data2[i] * data2[i]);
    }
}
// sse
void simd_sqrt_calc(float* data1, float* data2, int len, float out[])
{
    // g++ -msse3 -O3 -Wall -lrt sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20
    // g++ sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20

    assert((len % 4) == 0);
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
// avx
void simd256_sqrt_calc(float* data1, float* data2, int len, float out[])
{
    assert((len % 8) == 0);
    // AVX g++ cpmpile cmd: g++ -mavx sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20
    // AVX g++ cpmpile cmd: g++ -march=native sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20

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
        // _mm256_storeu_ps
        a++;
        b++;
        res++;
    }
}
// sse simd 计算 float类型的vector4和4x4矩阵的乘法
void vec4_mul_mat4(const float vd[4], const std::array<float[4], 4>& md, float out[4])
{
    __m128* v  = (__m128*)vd;
    __m128  i0 = *((__m128*)md[0]);
    __m128  i1 = *((__m128*)md[1]);
    __m128  i2 = *((__m128*)md[2]);
    __m128  i3 = *((__m128*)md[3]);

    __m128 m0 = _mm_mul_ps(*v, i0);
    __m128 m1 = _mm_mul_ps(*v, i1);
    __m128 m2 = _mm_mul_ps(*v, i2);
    __m128 m3 = _mm_mul_ps(*v, i3);

    __m128 u0 = _mm_unpacklo_ps(m0, m1);
    __m128 u1 = _mm_unpackhi_ps(m0, m1);
    __m128 a0 = _mm_add_ps(u0, u1);

    __m128 u2 = _mm_unpacklo_ps(m2, m3);
    __m128 u3 = _mm_unpackhi_ps(m2, m3);
    __m128 a1 = _mm_add_ps(u2, u3);

    __m128 f0 = _mm_movelh_ps(a0, a1);
    __m128 f1 = _mm_movehl_ps(a1, a0);
    __m128 f2 = _mm_add_ps(f0, f1);
#ifdef __GNUC__
    std::memcpy(out, &f2, sizeof(float) * 4);
#else
    std::memcpy(out, f2.m128_f32, sizeof(float) * 4);
#endif
}
void apply_align_malloc(size_t nBytes, void** ptrPtr)
{
    int boundary = 32;
    // posix_memalign( ptrPtr, boundary, nBytes );
}
} // namespace
void test_sqrt_calc()
{
    std::cout << "\n... test_sqrt_calc() begin ..." << std::endl;
    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_real_distribution<float> distribute(100.5f, 20001.5f);
    constexpr int                         data_size = 8192 << 12;
    // 应用__attribute__ ((aligned (32))) 语法解决 avx linux 运行时内存对齐问题，
    // 如果没有强制对齐，则会出现运行时 Segmentation fault 错误
    // 在 MSVC环境 则用: __declspec(align(32))

#ifdef __GNUC__
    // invalid attribute defined syntax in linux heap run time memory env, make runtime avx error: Segmentation fault
    // __attribute__ ((aligned (32))) float           *data1 = new float[data_size]{};
    // __attribute__ ((aligned (32))) float           *data2 = new float[data_size]{};
    // __attribute__ ((aligned (32))) float           *data_out = new float[data_size]{};
    //
    // valid attribute defined syntax in linux heap run time memory env, runtime correct.
    float* data1    = new float __attribute__((aligned(32)))[data_size]{};
    float* data2    = new float __attribute__((aligned(32)))[data_size]{};
    float* data_out = new float __attribute__((aligned(32)))[data_size]{};

    // valid attribute  defined syntax in linux code stack run time memory env, runtime correct.
    // __attribute__ ((aligned (32))) float data1[data_size]{};
    // __attribute__ ((aligned (32))) float data2[data_size]{};
    // __attribute__ ((aligned (32))) float data_out[data_size]{};
#else
    float*                  data1    = new float[data_size]{};
    float*                  data2    = new float[data_size]{};
    float*                  data_out = new float[data_size]{};
#endif
    std::cout << "elements total: " << data_size << std::endl;
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
    for (auto i = 0; i < 1; ++i)
    {
        normal_sqrt_calc(data1, data2, data_size, data_out); // debug 146ms, release 22ms, data_size = 8192 << 12, in Win10 MSVC;
        //simd_sqrt_calc(data1, data2, data_size, data_out);// debug 38ms, release 23ms
        //simd256_sqrt_calc(data1, data2, data_size, data_out); //debug 24ms, release 23ms
        // 目前的代码中的buffer数据定义和使用，违背cpu cache的空间局部性原则，
        // 导致更多的cpu cache miss。如果改为SOA(struct-of-array), 效率更高，
        // 当前的计算过程relase情况下耗时能减少1/3。
        // 目前的代码中的buffer数据定义和使用，看起来与cpu cache的空间局部性原则冲突，
        // 可能导致较多的cpu cache miss，但是鉴于这个用例每次读到的都是新数据，
        // 则另当别论。
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "loss time: " << lossTime << "ms" << std::endl;

    std::cout << "data_out: ";
    for (int i = 0; i < 8; ++i)
    {
        std::cout << data_out[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "... test_sqrt_calc() end ..." << std::endl;
}
void test_matrix_calc()
{
    std::cout << "\n... test_matrix_calc() begin ...\n" << std::endl;
#ifdef __GNUC__

    __attribute__((aligned(32))) float                   vec4_out[4]{};
    __attribute__((aligned(32))) float                   vec4_01[4]{1.1f, 2.2f, 3.3f, 1.0f};
    __attribute__((aligned(32))) std::array<float[4], 4> mat4_01{
        {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
#else
    float                   vec4_out[4]{};
    float                   vec4_01[4]{1.1f, 2.2f, 3.3f, 1.0f};
    std::array<float[4], 4> mat4_01{
        {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
#endif
    vec4_mul_mat4(vec4_01, mat4_01, vec4_out);

    std::cout << "SSE vec4_mul_mat4 :" << vec4_out[0] << "," << vec4_out[1] << "," << vec4_out[2] << "," << vec4_out[3] << std::endl;
    std::cout << "\n... test_matrix_calc() end ..." << std::endl;
}
void testBase()
{
    //g++ -msse3 -O3 -Wall -lrt checkSSEAVX2.cc -o check2.out -std=c++20
    std::cout << "\n... testBase() begin ...\n"
              << std::endl;
    ///*
#ifdef __GNUC__
    const __m128 zero = _mm_setzero_ps();
    const __m128 eq   = _mm_cmpeq_ps(zero, zero);
    const int    mask = _mm_movemask_ps(eq);
    std::cout << "testBase(), mask: " << mask << std::endl;
    union
    {
        __m128               v;
        float                vs[4]{4.0f, 4.1f, 4.2f, 4.3f};
        std::array<float, 4> array;
    } SIMD4Data{};
    __m128 a4   = _mm_set_ps(4.0f, 4.1f, 4.2f, 4.3f);
    __m128 b4   = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
    __m128 sum4 = _mm_add_ps(a4, b4);
    auto   f_vs = (float*)&sum4;
    std::cout << "SSE sum4 A:" << f_vs[0] << "," << f_vs[1] << "," << f_vs[2] << "," << f_vs[3] << std::endl;
    // 获取值的顺序和你输入的顺序是相反的
    std::cout << "SSE sum4 B:" << f_vs[3] << "," << f_vs[2] << "," << f_vs[1] << "," << f_vs[0] << std::endl;
    SIMD4Data.v = _mm_add_ps(a4, b4);
    std::cout << "SSE Sum SIMD4Data.vs:" << SIMD4Data.vs[0] << "," << SIMD4Data.vs[1] << "," << SIMD4Data.vs[2] << "," << SIMD4Data.vs[3] << std::endl;
    float result_vs[4]{};
    _mm_store_ps(result_vs, sum4);
    std::cout << "SSE Sum result_vs:" << result_vs[0] << "," << result_vs[1] << "," << result_vs[2] << "," << result_vs[3] << std::endl;
#else
    union
    {
        __m128               v;
        float                vs[4]{4.0f, 4.1f, 4.2f, 4.3f};
        std::array<float, 4> array;
    } SIMD4Data{};
    __m128 a4   = _mm_set_ps(4.0f, 4.1f, 4.2f, 4.3f);
    __m128 b4   = _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
    __m128 sum4 = _mm_add_ps(a4, b4);
    std::cout << "SSE sum4.m128_f32 A:" << sum4.m128_f32[0] << "," << sum4.m128_f32[1] << "," << sum4.m128_f32[2] << "," << sum4.m128_f32[2] << std::endl;
    // 获取值的顺序和你输入的顺序是相反的
    std::cout << "SSE sum4.m128_f32 B:" << sum4.m128_f32[3] << "," << sum4.m128_f32[2] << "," << sum4.m128_f32[1] << "," << sum4.m128_f32[0] << std::endl;
    std::cout << "SSE sum4.m128_f32[0]:" << sum4.m128_f32[0] << std::endl;
    SIMD4Data.v = _mm_add_ps(a4, b4);
    std::cout << "SSE Sum SIMD4Data.vs:" << SIMD4Data.vs[0] << "," << SIMD4Data.vs[1] << "," << SIMD4Data.vs[2] << "," << SIMD4Data.vs[3] << std::endl;
    std::cout << "SSE Sum SIMD4Data.array:" << SIMD4Data.array[0] << "," << SIMD4Data.array[1] << "," << SIMD4Data.array[2] << "," << SIMD4Data.array[3] << std::endl;

    std::cout << "\n";
    union
    {
        __m256 v;
        float  vs[8]{};
    } SIMD8Data{};

    __m256 a8   = _mm256_set_ps(4.0f, 4.1f, 4.2f, 4.3f, 4.0f, 4.1f, 4.2f, 4.3f);
    __m256 b8   = _mm256_set_ps(3.0f, 3.3f, 3.2f, 3.3f, 3.0f, 3.1f, 3.2f, 3.3f);
    __m256 sum8 = _mm256_add_ps(a8, b8);
    SIMD8Data.v = _mm256_add_ps(a8, b8);
    std::cout << "SSE sum8.m256_f32:" << sum8.m256_f32[0] << "," << sum8.m256_f32[1] << "," << sum8.m256_f32[2] << std::endl;
    std::memcpy(SIMD8Data.vs, sum8.m256_f32, sizeof(float));
    std::cout << "SSE SIMD8Data.vs:" << SIMD8Data.vs[0] << "," << SIMD8Data.vs[1] << "," << SIMD8Data.vs[2] << std::endl;
#endif
    std::cout << "\n... testBase() end ..." << std::endl;
}
void testBase2()
{
    std::cout << "\n... testBase2() begin ...\n";
#ifdef __GNUC__
    float v0[4] __attribute__((aligned(16))) = {0.0f, 1.0f, 2.0f, 3.0f};
    float v1[4] __attribute__((aligned(16))) = {4.0f, 5.0f, 6.0f, 7.0f};
#else
    float v0[4] = {0.0f, 1.0f, 2.0f, 3.0f};
    float v1[4] = {4.0f, 5.0f, 6.0f, 7.0f};
#endif
    /*
     _MM_SHUFFLE macro is supported in both Linux GCC and Windows MSVC, in linux gcc, defined in gcc\config\i386\xmmintrin.h head file.

     __m128 _mm_shuffle_ps(__m128 lo,__m128 hi,_MM_SHUFFLE(hi3,hi2,lo1,lo0)):
     Interleave inputs into low 2 floats and high 2 floats of output. Basically
     out[0]=lo[lo0];
     out[1]=lo[lo1];
     out[2]=hi[hi2];
     out[3]=hi[hi3];
     For example, _mm_shuffle_ps(a,a,_MM_SHUFFLE(i,i,i,i)) copies the float a[i] into all 4 output floats.
    */
    __m128 r0 = _mm_load_ps(v0);
    __m128 r1 = _mm_load_ps(v1);
    __m128 r1_b = _mm_set_ps(v1[0], v1[1], v1[2], v1[3]);
    _mm_store_ps(v0, r1);
    std::cout << "SSE r1 :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    _mm_store_ps(v0, r1_b);
    std::cout << "SSE r1_b :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    auto r1_b_0 = _mm_shuffle_ps(r1_b, r1_b, _MM_SHUFFLE(3, 2, 1, 0));
    _mm_store_ps(v0, r1_b_0);
    std::cout << "SSE r1_b_0 :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    // output: SSE r1_b_0 : 7, 6, 5, 4
    auto r1_b_1 = _mm_shuffle_ps(r1_b, r1_b, _MM_SHUFFLE(0, 1, 2, 3));
    _mm_store_ps(v0, r1_b_1);
    std::cout << "SSE r1_b_1 :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    // output: SSE r1_b_1 : 4, 5, 6, 7

    // Identity: r2 = r0.
    std::cout << "Identity:" << std::endl;
    auto r2 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(3, 2, 1, 0));
    _mm_store_ps(v0, r2);
    std::cout << "SSE r2 :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    auto r2_b = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3, 2, 1, 0));
    _mm_store_ps(v0, r2);
    std::cout << "SSE r2_b :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;
    auto r2_c = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3, 2, 1, 0));
    _mm_store_ps(v0, r2);
    std::cout << "SSE r2_c :" << v0[0] << "," << v0[1] << "," << v0[2] << "," << v0[3] << std::endl;

    _mm_store_ps(v0, r2);
    for (auto x : v0) { std::cout << x << ' '; }
    std::cout << std::endl;

    // Reverse r0
    std::cout << "Reverse r0:" << std::endl;
    r2 = _mm_shuffle_ps(r0, r0, _MM_SHUFFLE(0, 1, 2, 3));

    _mm_store_ps(v0, r2);
    for (auto x : v0) { std::cout << x << ' '; }
    std::cout << std::endl;

    // Even: { r0[ 0 ], r0[ 2 ], r1[ 0 ], r1[ 2 ] }
    std::cout << "Even: { r0[ 0 ], r0[ 2 ], r1[ 0 ], r1[ 2 ] }" << std::endl;
    r2 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(2, 0, 2, 0));

    _mm_store_ps(v0, r2);
    for (auto x : v0) { std::cout << x << ' '; }
    std::cout << std::endl;

    // Misc: { r0[ 3 ], r0[ 2 ], r1[ 2 ], r1[ 1 ] }
    std::cout << "Misc: { r0[ 3 ], r0[ 2 ], r1[ 2 ], r1[ 1 ] }" << std::endl;
    r2 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1, 2, 2, 3));

    _mm_store_ps(v0, r2);
    for (auto x : v0) { std::cout << x << ' '; }
    std::cout << std::endl;
    std::cout << "\n... testBase2() end ..." << std::endl;
}
} // namespace sseavx::test