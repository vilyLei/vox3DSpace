#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>
#include <tuple>
#include <functional>
#include <memory>

namespace base
{
namespace typeInfo
{

namespace mem_fn_demo
{
struct Foo
{
    void display_greeting()
    {
        std::cout << "Hello, world.\n";
    }

    void display_number(int i)
    {
        std::cout << "number: " << i << '\n';
    }

    int add_xy(int x, int y)
    {
        return data + x + y;
    }

    template <typename... Args> int add_many(Args... args)
    {
        return data + (args + ...);
    }

    auto add_them(auto... args)
    {
        return data + (args + ...);
    }

    int data = 7;
};

int testMain()
{
    auto f = Foo{};

    auto greet = std::mem_fn(&Foo::display_greeting);
    greet(f);

    auto print_num = std::mem_fn(&Foo::display_number);
    print_num(f, 42);

    auto access_data = std::mem_fn(&Foo::data);
    std::cout << "data: " << access_data(f) << '\n';

    auto add_xy = std::mem_fn(&Foo::add_xy);
    std::cout << "add_xy: " << add_xy(f, 1, 2) << '\n';

    auto u = std::make_unique<Foo>();
    std::cout << "access_data(u): " << access_data(u) << '\n';
    std::cout << "add_xy(u, 1, 2): " << add_xy(u, 1, 2) << '\n';

    auto add_many = std::mem_fn(&Foo::add_many<short, int, long>);
    std::cout << "add_many(u, ...): " << add_many(u, 1, 2, 3) << '\n';

    auto add_them = std::mem_fn(&Foo::add_them<short, int, float, double>);
    std::cout << "add_them(u, ...): " << add_them(u, 5, 7, 10.0f, 13.0) << '\n';
    return 1;
}
}
namespace demo_01
{

std::tuple<double, char, std::string> get_student(int id)
{
    switch (id)
    {
        case 0: return {3.8, 'A', "Lisa Simpson"};
        case 1: return {2.9, 'C', "Milhouse Van Houten"};
        case 2: return {1.7, 'D', "Ralph Wiggum"};
        case 3: return {0.6, 'F', "Bart Simpson"};
    }

    throw std::invalid_argument("id");
}

int testMain02()
{
    const auto student0 = get_student(0);
    std::cout << "ID: 0, "
              << "GPA: " << std::get<0>(student0) << ", "
              << "grade: " << std::get<1>(student0) << ", "
              << "name: " << std::get<2>(student0) << '\n';

    const auto student1 = get_student(1);
    std::cout << "ID: 1, "
              << "GPA: " << std::get<double>(student1) << ", "
              << "grade: " << std::get<char>(student1) << ", "
              << "name: " << std::get<std::string>(student1) << '\n';

    double      gpa2;
    char        grade2;
    std::string name2;
    std::tie(gpa2, grade2, name2) = get_student(2);
    std::cout << "ID: 2, "
              << "GPA: " << gpa2 << ", "
              << "grade: " << grade2 << ", "
              << "name: " << name2 << '\n';

    // C++17 structured binding:
    const auto [gpa3, grade3, name3] = get_student(3);
    std::cout << "ID: 3, "
              << "GPA: " << gpa3 << ", "
              << "grade: " << grade3 << ", "
              << "name: " << name3 << '\n';
    return 1;
}
void testMain()
{
    int         i = 33;
    decltype(i) j = i * 2;

    std::cout << "i and j are the same type? " << std::boolalpha
              << std::is_same_v<decltype(i), decltype(j)> << '\n';

    std::cout << "i = " << i << ", "
              << "j = " << j << '\n';
    auto f = [](int a, int b) -> int {
        return a * b;
    };

    decltype(f) g = f; // the type of a lambda function is unique and unnamed
    i             = f(2, 2);
    j             = g(3, 3);

    std::cout << "i = " << i << ", "
              << "j = " << j << '\n';
}
} // namespace demo_01

void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::typeInfo::testMain() begin.\n";
    demo_01::testMain();
    demo_01::testMain02();
    mem_fn_demo::testMain();
    std::cout << "base::typeInfo::testMain() end.\n";
}
} // namespace typeInfo
} // namespace base