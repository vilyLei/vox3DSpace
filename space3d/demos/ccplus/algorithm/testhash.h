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
#include <tuple>
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
std::tuple<int, double> tuple_func()
{
    return std::tuple(1, 2.2);
}
void testMain2()
{
    // Create an unordered_map of three strings (that map to strings)
    std::unordered_map<std::string, std::string> u =
        {
            {"RED", "#FF0000"},
            {"GREEN", "#00FF00"},
            {"BLUE", "#0000FF"}};
    u["ss"] = "ss_ctx";
    // Helper lambda function to print key-value pairs
    auto print_key_value = [](const auto& key, const auto& value) {
        std::cout << "Key:[" << key << "] Value:[" << value << "]\n";
    };

    std::cout << "Iterate and print key-value pairs of unordered_map, being\n"
                 "explicit with their types:\n";
    for (const std::pair<const std::string, std::string>& n : u)
        print_key_value(n.first, n.second);

    std::cout << "\nIterate and print key-value pairs using C++17 structured binding:\n";
    for (const auto& [key, value] : u)
        print_key_value(key, value);

    // Add two new entries to the unordered_map
    u["BLACK"] = "#000000";
    u["WHITE"] = "#FFFFFF";

    std::cout << "\nOutput values by key:\n"
                 "The HEX of color RED is:["
              << u["RED"] << "]\n"
                             "The HEX of color BLACK is:["
              << u["BLACK"] << "]\n\n";

    std::cout << "Use operator[] with non-existent key to insert a new key-value pair:\n";
    print_key_value("new_key", u["new_key"]);

    std::cout << "\nIterate and print key-value pairs, using `auto`;\n"
                 "new_key is now one of the keys in the map:\n";
    for (const auto& n : u)
        print_key_value(n.first, n.second);
}
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

    std::string                             str01{};
    std::unordered_map<size_t, std::string> map01{};
    str01 = "world_pass";
    auto hashKey = std::hash<std::string>{}(str01);
    map01[hashKey] = str01;

    std::cout << "map01[hashKey]: " << map01[hashKey] << '\n';

    auto [t_a, t_b] = tuple_func();

    std::cout << "XXXX t_a: " << t_a << '\n';
    std::cout << "XXXX t_b: " << t_b << '\n';
}
} // namespace thash01
void testMain()
{
    std::cout << "### testhash testMain(), begin ...\n";
    thash01::testMain();
    std::cout << "### testhash testMain(), end ...\n";
}
} // namespace algorithm::testsort