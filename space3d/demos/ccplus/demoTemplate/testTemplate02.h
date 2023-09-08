#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace demoTemplate::base02
{
namespace templateDemo_b01
{

template <auto...>
struct C
{};

C<'C', 0, 2L, nullptr> x; // OK

void testMain()
{
}
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "demoTemplate::base02::testMain() begin.\n";
    templateDemo_b01::testMain();
    std::cout << "demoTemplate::base02::testMain() end.\n";
}
} // namespace demoTemplate::base