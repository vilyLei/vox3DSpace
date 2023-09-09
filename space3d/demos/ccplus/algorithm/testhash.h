#pragma once

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <vector>
#include <iterator>
#include <random>
#include <list>
#include <thread>
#include <functional>
#include <cstddef>
#include <iomanip>
#include <string>
#include <unordered_set>
#include <unordered_map>


namespace std
{
struct S
{
    std::string first_name;
    std::string last_name;
    bool        operator==(const S&) const = default; // since C++20
};

// Before C++20.
 //bool operator==(const S& lhs, const S& rhs)
 //{
 //    return lhs.first_name == rhs.first_name && lhs.last_name == rhs.last_name;
 //}

// Custom hash can be a standalone function object.
struct MyHash
{
    std::size_t operator()(const S& s) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(s.first_name);
        std::size_t h2 = std::hash<std::string>{}(s.last_name);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};

// Custom specialization of std::hash can be injected in namespace std.
template <>
struct std::hash<S>
{
    std::size_t operator()(const S& s) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(s.first_name);
        std::size_t h2 = std::hash<std::string>{}(s.last_name);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};
} // namespace thash01
namespace algorithm::testhash
{
size_t compute_hash(std::string const& s)
{
    // thanks: https://cp-algorithms.com/string/string-hashing.html#calculation-of-the-hash-of-a-string
    const int p          = 31;
    const int m          = 1e9 + 9;
    long long hash_value = 0;
    long long p_pow      = 1;
    for (char c : s)
    {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow      = (p_pow * p) % m;
    }
    return hash_value;
}
namespace thash01
{
using namespace std;
void testMain()
{
    std::string str      = "Meet the new boss...";
    std::size_t str_hash = std::hash<std::string>{}(str);
    std::cout << "hash(" << std::quoted(str) << ") =\t" << str_hash << '\n';

    S obj = {"Hubert", "Farnsworth"};
    // Using the standalone function object.
    std::cout << "hash(" << std::quoted(obj.first_name) << ", "
              << std::quoted(obj.last_name) << ") =\t"
              << MyHash{}(obj) << " (using MyHash) or\n\t\t\t\t"
              << std::hash<S>{}(obj) << " (using injected specialization)\n";

    // Custom hash makes it possible to use custom types in unordered containers.
    // The example will use the injected std::hash<S> specialization above,
    // to use MyHash instead, pass it as a second template argument.
    std::unordered_set<S> names = {obj, {"Bender", "Rodriguez"}, {"Turanga", "Leela"}};
    for (auto const& s : names)
        std::cout << std::quoted(s.first_name) << ' '
                  << std::quoted(s.last_name) << '\n';
}
} // namespace thash01
void testMain()
{
    std::cout << "### testhash testMain(), begin ...\n";
    thash01::testMain();
    std::cout << "### testhash testMain(), end ...\n";
}
} // namespace algorithm::testsort