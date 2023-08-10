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

namespace sseavx::testwithsoa
{
namespace
{
// function meaning: value = sqrt(a*a + b*b)
void normal_sqrt_calc(float data[], int len, float out[])
{
    int i{};
    int j{};
    for (i = 0; i < len; i+=2)
    {
        //out[j++] = (data[i] * data[i] + data[i+1] * data[i+1]);
        out[j++] = sqrt(data[i] * data[i] + data[i+1] * data[i+1]);
    }
}
// sse
void simd_sqrt_calc(float* data, int len, float out[])
{
    // g++ -msse3 -O3 -Wall -lrt sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20
    // g++ sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20
    assert((len % 4) == 0);
    __m128 *a, *b, *res, t1, t2, t3; // = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
    int     i, tlen = len / 4;

    a   = (__m128*)data;
    b   = (__m128*)(data + 4);
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
void simd256_sqrt_calc(float* data, int len, float out[])
{
    assert((len % 8) == 0);
    // AVX g++ cpmpile cmd: g++ -mavx sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20
    // AVX g++ cpmpile cmd: g++ -march=native sseavxBaseTest.cc -o sseavxBaseTest.out -std=c++20

    __m256 *a, *b, *res, t1, t2, t3; // = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
    int     i, tlen = len / 8;
    a   = (__m256*)data;
    b   = (__m256*)(data + 8);
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
    std::cout << "\n... soa test_sqrt_calc() begin ..." << std::endl;
    std::random_device                    rd;
    std::mt19937                          gen(rd());
    std::uniform_real_distribution<float> distribute(100.5f, 20001.5f);
    //constexpr int total     = 8192 << 12;
    constexpr int total     = 8192 << 8;
    constexpr int data_size = total << 1;// data total size is 128M * 3 = 384M

    // 应用__attribute__ ((aligned (32))) 语法解决 avx linux 运行时内存对齐问题，
    // 如果没有强制对齐，则会出现运行时 Segmentation fault 错误
    // 在 MSVC环境 则用: __declspec(align(32))

#ifdef __GNUC__
    // valid attribute defined syntax in linux heap run time memory env, runtime correct.
    float* data    = new float __attribute__((aligned(32)))[data_size]{};
    float* data_out = new float __attribute__((aligned(32)))[total]{};
#else
    float*                  data    = new float[data_size]{};
    float*                  data_out = new float[total]{};
#endif
    std::cout << "elements total: " << total << std::endl;
    //std::cout << "data: ";
    for (int i = 0; i < data_size; ++i)
    {
        auto v   = distribute(gen);
        data[i] = v;
        //if (i < 8)
        //    std::cout << distribute(gen) << " ";
    }
    std::cout << std::endl;

    auto time_start = std::chrono::high_resolution_clock::now();

    for (auto i = 0; i < 50; ++i)
    {
        normal_sqrt_calc(data, total, data_out);// debug 146ms, release 16, data_size = 8192 << 12, in Win10 MSVC;
        //simd_sqrt_calc(data, total, data_out); // debug 37ms, release 19ms
        //simd256_sqrt_calc(data, total, data_out); //debug 24ms, release 16ms
        // 目前的代码cpu cache miss比较少，效率更高
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "soa loss time: " << lossTime << "ms" << std::endl;

    //std::cout << "data_out: ";
    //for (int i = 0; i < 8; ++i)
    //{
    //    std::cout << data_out[i] << " ";
    //}
    //std::cout << std::endl;
    std::cout << "... test_sqrt_calc() end ..." << std::endl;
}
void test_matrix_calc()
{
    std::cout << "\n... soa test_matrix_calc() begin ...\n" << std::endl;
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
} // namespace sseavx::test