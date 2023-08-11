#include <iostream>
#include <list>
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>

namespace parallel::futuresort
{
// 若干个不同数的全排列
namespace permutation
{
void static swapInt(int* M, int* N)
{
    int tem = *M;
    *M      = *N;
    *N      = tem;
}

void f6(int a[], int k, int length) //k表示起始位置，length表示末尾位置
{
    int i = 0;
    if (k == length)
    {
        //for (i = 0; i <= length; i++)
        //{
        //    printf("%d", a[i]);
        //}
        //printf("\n");
    }
    else
    {
        for (i = k; i <= length; i++)
        {
            swapInt(&a[k], &a[i]);
            f6(a, k + 1, length);
            swapInt(&a[k], &a[i]);
        }
    }
}

int testMain()
{
    int a[]    = {1, 2, 3, 4, 5};
    int length = sizeof(a) / sizeof(a[0]) - 1;
    f6(a, 0, length);
    system("pause");
    return 0;
}
} // namespace permutation
namespace testpartition
{

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
    index       = static_cast<int>(dis_it_va01);

    std::cout << "\niterator index(in the vector): " << index << std::endl;
    auto nextIt = std::next(va01.begin(), static_cast<ptrdiff_t>(3));
    auto ti     = static_cast<int>(static_cast<ptrdiff_t>(3));
    std::cout << "\n(static_cast<ptrdiff_t>(3)): " << ti << std::endl;
    std::cout << "\nstd::next(it_va01, static_cast<ptrdiff_t>(3)) value: " << *nextIt << std::endl;
    nextIt = std::next(va01.begin(), static_cast<ptrdiff_t>(5) / 3);
    ti     = static_cast<int>(static_cast<ptrdiff_t>(5) / 3);
    std::cout << "\n(static_cast<ptrdiff_t>(5) / 3): " << ti << std::endl;
    std::cout << "\nstd::next(it_va01, static_cast<ptrdiff_t>(5)/3) value: " << *nextIt << std::endl;
}

template <typename TV>
void quicksortList3(TV t0, TV t1)
{
    if (t0 != t1)
    {
        auto value = *std::next(t0, (t1 - t0) / 2);
        auto p0    = std::partition(t0, t1, [value](const auto& v) {
            return v < value;
        });
        auto p1    = std::partition(p0, t1, [value](const auto& v) {
            return v <= value;
        });
        quicksortList3(t0, p0);
        quicksortList3(p1, t1);
    }
}

template <typename IteratorT>
void quicksortList2(IteratorT ia, IteratorT ib)
{
    if (ia != ib)
    {
        auto value = *std::next(ia, std::distance(ia, ib) / 2);
        auto pos1  = std::partition(ia, ib, [value](const auto& v) {
            return v < value;
        });
        auto pos2  = std::partition(pos1, ib, [value](const auto& v) {
            return v <= value;
        });
        quicksortList2(ia, pos1);
        quicksortList2(pos2, ib);
    }
}

template <typename iteratorT>
void quicksortList(iteratorT begin, iteratorT end)
{
    if (begin != end)
    {
        auto midValue = *std::next(begin, std::distance(begin, end) / 2);
        auto leftPos  = std::partition(begin, end, [midValue](const auto& v) {
            return v < midValue;
        });
        auto rightPos = std::partition(leftPos, end, [midValue](const auto& v) {
            return v <= midValue;
        });
        quicksortList(begin, leftPos);
        quicksortList(rightPos, end);
    }
}
template <class ForwardIt>
void quicksort(ForwardIt first, ForwardIt last)
{
    if (first == last)
        return;

    auto pivot   = *std::next(first, std::distance(first, last) / 2);
    auto middle1 = std::partition(first, last, [pivot](const auto& em) {
        return em < pivot;
    });
    //std::cout << "\n###:";
    //for (auto it = first; it != last; it++)
    //     std::cout << *it << ',';
    auto middle2 = std::partition(middle1, last, [pivot](const auto& em) {
        return !(pivot < em);
    });

    quicksort(first, middle1);
    quicksort(middle2, last);

    //std::cout << "\n";
    //std::cout << "\n*pivot: " << pivot << std::endl;
    //std::cout << "*middle1: " << *middle1 << std::endl;
    //std::cout << "*middle2: " << *middle2 << std::endl;
}
void testMain()
{
    int int_va = 19;
    int int_vb = 3;
    std::cout << "(int_va / int_vb): " << (int_va / int_vb) << std::endl;

    //testGetVectorIeratorIndex();
    //return;
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::cout << "Original vector: ";
    for (auto elem : v)
        std::cout << elem << ' ';

    //auto it = std::partition(v.begin(), v.end(), [](int i) { return i % 2 == 0; });
    auto it = std::partition(v.begin(), v.begin() + 5, [](int i) { return i % 2 == 0; });
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

    std::forward_list<int> fl{1, 30, -4, 3, 5, 8, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::forward_list<int> f2{1, 30, -4, 3, 5, 8, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::list<int>         list1{1, 30, -4, 3, 5, 8, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::vector<int>       vector1{1, 30, -4, 3, 5, 8, -4, 1, 6, -8, 2, -5, 64, 1, 92};

    std::cout << "\nAAA Unsorted list: ";
    for (int n : fl)
        std::cout << n << ' ';

    quicksort(std::begin(fl), std::end(fl));
    std::cout << "\nBBB Sorted using quicksort: ";
    for (int fi : fl)
        std::cout << fi << ' ';
    std::cout << '\n';

    std::cout << "\nCCC old f2: ";
    for (int fi : f2)
        std::cout << fi << ' ';
    std::cout << '\n';
    quicksortList(std::begin(f2), std::end(f2));

    std::cout << "\nCCC quicksort new f2: ";
    for (int fi : f2)
        std::cout << fi << ' ';
    std::cout << '\n';

    std::cout << "\nDDD old list1: ";
    for (int fi : list1)
        std::cout << fi << ' ';
    std::cout << '\n';
    quicksortList(std::begin(list1), std::end(list1));
    std::cout << "\nDDD quicksort new list1: ";
    for (int fi : list1)
        std::cout << fi << ' ';
    std::cout << '\n';

    std::cout << "\nEEE old vector1: ";
    for (int fi : vector1)
        std::cout << fi << ' ';
    std::cout << '\n';
    //quicksortList(std::begin(vector1), std::end(vector1));
    //quicksortList2(std::begin(vector1), std::end(vector1));
    quicksortList3(std::begin(vector1), std::end(vector1));
    std::cout << "\nEEE quicksort new vector1: ";
    for (int fi : list1)
        std::cout << fi << ' ';
    std::cout << '\n';
}
} // namespace testpartition
namespace pallfastsort
{
template <typename T>
std::list<T> sequential_quick_sort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }
    std::list<T> result;

    //auto ia = std::next(input.begin(), 3);
    //auto ib = std::next(input.begin(), 6);
    //result.splice(result.begin(), input, input.begin()); // 1
    //result.splice(result.begin(), input, ia, ib); // 1
    result.splice(result.begin(), input, input.begin()); // 1

    //std::cout << "\nAAA 0 result: ";
    //for (auto fi : result)
    //    std::cout << fi << ' ';
    //std::cout << '\n';
    //std::cout << "\nAAA 0 input: ";
    //for (auto fi : input)
    //    std::cout << fi << ' ';
    //std::cout << '\n';

    T const& pivot = *result.begin(); // 2
    //auto         divide_point = std::partition(input.begin(), input.end(),
    //                                           [&](T const& t) { return t < pivot; }); // 3
    auto         divide_point = std::partition(input.begin(), input.end(),
                                       [&](const auto& t) { return t < pivot; }); // 3
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point); // 4

    //std::cout << "\nAAA 1 lower_part: ";
    //for (auto fi : lower_part)
    //    std::cout << fi << ' ';
    //std::cout << '\n';

    //std::cout << "\nAAA 1 input: ";
    //for (auto fi : input)
    //    std::cout << fi << ' ';
    //std::cout << '\n';

    auto new_lower(std::move(sequential_quick_sort(std::move(lower_part)))); // 5
    auto new_higher(std::move(sequential_quick_sort(std::move(input))));     // 6
    result.splice(result.end(), new_higher);                                 // 7
    result.splice(result.begin(), new_lower);                                // 8
    return result;
}
template <typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if (input.empty())
    {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const&     pivot        = *result.begin();
    auto         divide_point = std::partition(input.begin(), input.end(),
                                       [&](T const& t) { return t < pivot; });
    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    std::future<std::list<T>> new_lower( std::async(&parallel_quick_sort<T>, std::move(lower_part)) ); //	1
    auto new_higher( parallel_quick_sort(std::move(input)) ); // 2
    result.splice(result.end(), new_higher); //	3
    result.splice(result.begin(), new_lower.get()); //	4
    return result;
}

void testMain()
{
    std::list<int> list1{1, 30, -4, 3, 5, 8, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::cout << "\nDDD old list1: ";
    for (int fi : list1)
        std::cout << fi << ' ';
    std::cout << '\n';
    list1 = sequential_quick_sort(list1);
    std::cout << "\nDDD quicksort new list1: ";
    for (int fi : list1)
        std::cout << fi << ' ';
    std::cout << '\n';
}
} // namespace pallfastsort
void testMain()
{
    std::cout << "parallel::futuresort::testMain() begin.\n\n";
    //testpartition::testMain();
    pallfastsort::testMain();
    std::cout << "\nparallel::futuresort::testMain() begin.\n";
}
} // namespace parallel::futuresort