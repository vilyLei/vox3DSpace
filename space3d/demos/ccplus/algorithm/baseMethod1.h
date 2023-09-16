#include <iostream>
#include <chrono>

namespace algorithm::baseMethod1
{
namespace method1
{
int fib(int n)
{
    if (n == 0 || n == 1)
    {
        return n;
    }
    return fib(n - 2) + fib(n - 1);
}
void benchmarkFib(int total, int n)
{
    if (n > 0)
    {
        for (auto i = 0; i < total; ++i)
        {
            fib(n);
        }
    }
}
void testMain()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    benchmarkFib(2, 30);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto loss_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "benchmarkFib(), loss_time: " << loss_time << "ms" << std::endl;
}
}
void testMain()
{
    std::cout << "algorithm::baseMethod1::testMain(), begin ...\n";
    method1::testMain();
    std::cout << "algorithm::baseMethod1::testMain(), end ...\n";
}
}