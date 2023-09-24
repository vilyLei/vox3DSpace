#include <iostream>
#include <chrono>
#include <functional>
#include <memory>
#include <random>

namespace algorithm::baseMethod1
{
 namespace method2
 {
     void f(int n1, int n2, int n3, const int& n4, int n5)
     {
         std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
     }

     int g(int n1)
     {
         return n1;
     }

     struct Foo
     {
         void print_sum(int n1, int n2)
         {
             std::cout << n1 + n2 << '\n';
         }
         int data = 10;
     };

     void testMain()
     {
         using namespace std::placeholders;  // for _1, _2, _3...

         std::cout << "1) argument reordering and pass-by-reference: ";
         int n = 7;
         // (_1 and _2 are from std::placeholders, and represent future
         // arguments that will be passed to f1)
         auto f1 = std::bind(f, _2, 42, _1, std::cref(n), n);
         n = 11;
         f1(1, 2, 1001); // 1 is bound by _1, 2 is bound by _2, 1001 is unused
         // makes a call to f(2, 42, 1, n, 7)

         std::cout << "2) achieving the same effect using a lambda: ";
         n = 7;
         auto lambda = [&ncref = n, n](auto a, auto b, auto /*unused*/)
         {
             f(b, 42, a, ncref, n);
         };
         n = 10;
         lambda(1, 2, 1001); // same as a call to f1(1, 2, 1001)

         std::cout << "3) nested bind subexpressions share the placeholders: ";
         auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
         f2(10, 11, 12); // makes a call to f(12, g(12), 12, 4, 5);

         std::cout << "4) bind a RNG with a distribution: ";
         std::default_random_engine e;
         std::uniform_int_distribution<> d(0, 10);
         auto rnd = std::bind(d, e); // a copy of e is stored in rnd
         for (int n = 0; n < 10; ++n)
             std::cout << rnd() << ' ';
         std::cout << '\n';

         std::cout << "5) bind to a pointer to member function: ";
         Foo foo;
         auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
         f3(5);

         std::cout << "6) bind to a mem_fn that is a pointer to member function: ";
         auto ptr_to_print_sum = std::mem_fn(&Foo::print_sum);
         auto f4 = std::bind(ptr_to_print_sum, &foo, 95, _1);
         f4(5);

         std::cout << "7) bind to a pointer to data member: ";
         auto f5 = std::bind(&Foo::data, _1);
         std::cout << f5(foo) << '\n';

         std::cout << "8) bind to a mem_fn that is a pointer to data member: ";
         auto ptr_to_data = std::mem_fn(&Foo::data);
         auto f6 = std::bind(ptr_to_data, _1);
         std::cout << f6(foo) << '\n';

         std::cout << "9) use smart pointers to call members of the referenced objects: ";
         std::cout << f6(std::make_shared<Foo>(foo)) << ' '
             << f6(std::make_unique<Foo>(foo)) << '\n';
     }
 }
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
    method2::testMain();
    std::cout << "algorithm::baseMethod1::testMain(), end ...\n";
}
}