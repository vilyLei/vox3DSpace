#include <iostream>

namespace test_01
{
int a() { return std::puts("a"); }
int b() { return std::puts("b"); }
int c() { return std::puts("c"); }

void z(int, int, int) {}

int main02()
{
    // https://zh.cppreference.com/w/cpp/language/eval_order
    z(a(), b(), c());       // 允许全部 6 种输出排列
    return a() + b() + c(); // 允许全部 6 种输出排列
}
void foo(int a, int b)
{
    std::cout << a << "," << b << std::endl;
}
int get_num()
{
    static int i = 0;
    std::cout << "\t\ti: " << i+1 << std::endl;
    return ++i;
}
void testMain()
{
    std::cout << " ---------------------- -- a -- -------------------" << std::endl;
    foo(get_num(), get_num());
    foo(get_num(), get_num());
    std::cout << " ---------------------- -- b -- -------------------" << std::endl;
    main02();
    std::cout << " ---------------------- -- c -- -------------------" << std::endl;
}
}
int main(int argc, char** argv)
{
    test_01::testMain();
    return EXIT_SUCCESS;
}