#include <iostream>
#include <list>
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>

namespace parallel::futuresort
{
namespace testpartition
{

template <class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last)
{
    if (first == last)
        return;

    auto pivot   = *std::next(first, std::distance(first, last) / 2);
    auto middle1 = std::partition(first, last, [pivot](const auto& em) {
        return em < pivot;
    });
    auto middle2 = std::partition(middle1, last, [pivot](const auto& em) {
        return !(pivot < em);
    });

    quicksort(first, middle1);
    quicksort(middle2, last);
}
void testGetVectorIeratorIndex()
{
    //std::vector<double> va01{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<std::string> va01{"A-0y", "B-1", "C-2", "D-3uy7", "E-4", "F-yu5", "G-6", "H-jfgm7", "I-8", "J-9"};
    std::cout << "old vector: ";
    for (auto elem : va01)
        std::cout << elem << ',';
    //auto it_va01 = std::partition(va01.begin(), va01.end(), [](int i) { return i % 2 == 0; });
    auto it_va01 = std::partition(va01.begin(), va01.end(), [](std::string str) { return str.length() % 2 == 0; });
    //auto it = std::partition(v.begin(), v.end(), [](int i) { return i % 3 == 0; });
    std::cout << "\n\nnew vector: ";
    for (auto elem : va01)
        std::cout << elem << ',';
    std::cout << "\n\nvalue: " << *it_va01 << std::endl;
    auto dis_it_va01 = it_va01 - va01.begin();
    // dis_it_va01 是 ptrdiff_t类型, 而ptrdiff_t可能是 __int64 类型也可能是int,所以可以如下这样取转换
    auto index = static_cast<int>(dis_it_va01);
    std::cout << "\niterator index(in the vector): " << index << std::endl;

    dis_it_va01 = std::distance(va01.begin(), it_va01);
    index         = static_cast<int>(dis_it_va01);

    std::cout << "\niterator index(in the vector): " << index << std::endl;
    //auto nextIt = *std::next(va01.begin(), static_cast<ptrdiff_t>(3));
    auto nextIt = std::next(it_va01, static_cast<ptrdiff_t>(5)/3);
    std::cout << "\nstd::next(it_va01, static_cast<ptrdiff_t>(2)) value: " << *nextIt << std::endl;
}
void testMain()
{
    //testGetVectorIeratorIndex();
    //return;
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::cout << "Original vector: ";
    for (auto elem : v)
        std::cout << elem << ' ';

    auto it = std::partition(v.begin(), v.end(), [](int i) { return i % 2 == 0; });
    std::cout << "\nOriginal vector B: ";
    for (auto elem : v)
        std::cout << elem << ' ';
    std::cout << "\n*it: " << *it << std::endl;
    std::cout << "\n(it - v.begin()) index: " << static_cast<int>(it - v.begin()) << std::endl;
    //return;
    std::cout << "\nPartitioned vector: ";
    std::copy(std::begin(v), it, std::ostream_iterator<int>(std::cout, " "));
    std::cout << "* ";
    std::copy(it, std::end(v), std::ostream_iterator<int>(std::cout, " "));

    std::forward_list<int> fl{1, 30, -4, 3, 5, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::cout << "\nAAA Unsorted list: ";
    for (int n : fl)
        std::cout << n << ' ';

    quicksort(std::begin(fl), std::end(fl));
    std::cout << "\nBBB Sorted using quicksort: ";
    for (int fi : fl)
        std::cout << fi << ' ';
    std::cout << '\n';
}
} // namespace testpartition
namespace fastsort
{
template <typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin()); // 1
    T const&     pivot        = *result.begin();         // 2
    auto         divide_point = std::partition(input.begin(), input.end(),
                                               [&](T const& t) { return t < pivot; }); // 3
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(),
                      divide_point); // 4
    auto new_lower(
        sequential_quick_sort(std::move(lower_part))); // 5
    auto new_higher(
        sequential_quick_sort(std::move(input))); // 6
    result.splice(result.end(), new_higher);      // 7
    result.splice(result.begin(), new_lower);     // 8
    return result;
}

} // namespace fastsort
void testMain()
{
    std::cout << "parallel::futuresort::testMain() begin.\n\n";
    testpartition::testMain();
    std::cout << "\nparallel::futuresort::testMain() begin.\n";
}
} // namespace parallel::futuresort