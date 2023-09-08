#pragma once

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <vector>
#include <functional>
#include <iterator>
#include <random>
#include <list>
#include <thread>

namespace algorithm::testsort
{
template <typename Iter>
void printListWithIterRange(std::string info, Iter a, Iter b, std::string endStr = "\n")
{
    std::cout << info;
    auto it = a;
    if (it != b)
    {
        std::cout << *it;
        ++it;
    }
    for (; it != b; ++it)
    {
        std::cout << "," << *it;
    }
    std::cout << endStr;
}
namespace quicksort01
{

int sorting(std::vector<int>& arr, int low, int high)
{
    //标记位置为待排序数组段的low处也就时枢轴值
    auto& rsn = arr[low];
    while (low < high)
    {
        //  如果当前数字已经有序的位于我们的枢轴两端，我们就需要移动它的指针，是high或是low
        while (low < high && arr[high] >= rsn)
        {
            --high;
        }
        // 如果当前数字不满足我们的需求，我们就需要将当前数字移动到它应在的一侧
        arr[low] = arr[high];
        while (low < high && arr[low] <= rsn)
        {
            ++low;
        }
        arr[high] = arr[low];
    }
    arr[low] = rsn;
    return low;
}
void snsort(std::vector<int>& arr, int low, int high)
{
    if (low < high)
    {
        auto pos = sorting(arr, low, high);
        snsort(arr, low, pos - 1);
        snsort(arr, pos + 1, high);
    }
}
int quick_sort_partition(std::vector<int>& v, int low, int high)
{
    int pivot = v[low]; //记录第一个枢轴
    while (low < high)
    {
        while (low < high && v[high] >= pivot)
        {
            high--; // 找到第一个比pivot小的
        }
        v[low] = v[high]; // high指示小于pivot，交换
        while (low < high && v[low] <= pivot)
        {
            low++; // 找到第一个比pivot大的
        }
        v[high] = v[low]; // low指示大于pivot，交换
    }
    // 记录新的枢轴,一次partition后 pivot左边比pivot小。后边比pivot大
    v[low] = pivot;
    return low;
}

void quick_sort(std::vector<int>& v, int low, int high)
{
    if (low < high)
    {
        auto i = quick_sort_partition(v, low, high);
        quick_sort(v, low, i - 1);
        quick_sort(v, i + 1, high);
    }
}
} // namespace quicksort01
namespace tsort01
{
int callTimes = 0;

template <typename IT, typename FuncT>
IT listPartition2(IT a, IT b, FuncT comp)
{
    // bad implements ...
    auto it    = a;
    auto maxIt = a;
    auto rIT   = a;
    for (auto it = a; it != b; ++it)
    {
        if (comp(*it))
        {
            rIT = it;
            ++rIT;
        }
        else
        {
            maxIt = it;
            for (; ++maxIt != b;)
            {
                if (comp(*maxIt))
                {
                    std::iter_swap(it, maxIt);
                    rIT = it;
                    ++rIT;
                    break;
                }
            }
        }
    }
    return rIT;
}

template <typename IT, typename FuncT>
IT listPartition(IT a, IT b, FuncT comp)
{
    int total = 0;
    for (auto it = a; it != b; ++it)
    {
        total += comp(*it) ? 1 : 0;
    }
    auto it    = a;
    auto maxIt = std::next(a, total);
    auto rIT   = maxIt;
    // 将符合条件的, 紧凑的集中在一起
    for (; total > 0; --total)
    {
        if (!comp(*it))
        {
            for (; maxIt != b;)
            {
                if (comp(*maxIt))
                {
                    std::iter_swap(it, maxIt);
                    break;
                }
                ++maxIt;
            }
        }
        ++it;
    }
    return rIT;
}

template <typename IT, typename FuncT>
IT qpartition(IT a, IT b, FuncT comp)
{
    int total = 0;
    for (auto it = a; it != b; ++it)
    {
        if (comp(*it))
        {
            total++;
        }
    }
    auto maxIT = std::next(a, total);
    // 将符合条件的, 紧凑的集中在一起
    for (auto it = a; it != b; ++it)
    {
        if (total > 0)
        {
            if (!comp(*it))
            {
                for (auto it1 = maxIT; it1 != b; ++it1)
                {
                    //std::cout << " ooo *it: " << *it << ", *it1: " << *it1 << std::endl;
                    if (comp(*it1))
                    {
                        std::iter_swap(it, it1);
                        break;
                    }
                }
            }
        }
        else
        {
            break;
        }
        --total;
    }
    return maxIT;
}
template <typename IT>
void qsort2(IT a, IT b)
{
    if (a != b)
    {
        auto value = *std::next(a, std::distance(a, b) / 2);
        auto p0    = qpartition(a, b, [value](const auto& v) -> bool {
            return v < value;
        });
        auto p1    = qpartition(p0, b, [value](const auto& v) -> bool {
            return v <= value;
        });
        qsort2(a, p0);
        qsort2(p1, b);
    }
}
template <typename IT>
void listQSort2(IT a, IT b)
{
    if (a != b)
    {
        auto value = *std::next(a, std::distance(a, b) / 2);
        auto p0    = listPartition2(a, b, [value](const auto& v) -> bool {
            return v < value;
        });
        auto p1    = listPartition2(p0, b, [value](const auto& v) -> bool {
            return v <= value;
        });
        listQSort(a, p0);
        listQSort(p1, b);
    }
}
template <typename IT>
void listQSort(IT a, IT b)
{
    if (a != b)
    {
        auto value = *std::next(a, std::distance(a, b) / 2);
        auto p0    = listPartition(a, b, [value](const auto& v) -> bool {
            return v < value;
        });
        auto p1    = listPartition(p0, b, [value](const auto& v) -> bool {
            return v <= value;
        });
        listQSort(a, p0);
        listQSort(p1, b);
    }
}
template <typename IT>
void listQSortFast(IT a, IT b)
{
    if (a != b)
    {
        auto value = *std::next(a, std::distance(a, b) / 2);
        auto p0    = listPartition(a, b, [value](const auto& v) -> bool {
            return v < value;
        });
        listQSort(a, p0);
        listQSort(p0, b);
    }
}

template <typename IT>
void qsort1(IT a, IT b)
{
    if (a != b)
    {
        auto value = *std::next(a, std::distance(a, b) / 2);
        auto p0    = std::partition(a, b, [value](const auto& v) -> bool {
            return v < value;
        });
        auto p1    = std::partition(p0, b, [value](const auto& v) -> bool {
            return v <= value;
        });
        qsort1(a, p0);
        qsort1(p1, b);
    }
}
std::forward_list<int> createRandomList(int data_size = 20)
{
    std::random_device rd;
    std::mt19937       gen(rd());
    //std::uniform_real_distribution<double> distribute(0.0, 200.0);
    std::uniform_int_distribution<int> distribute(-100, 200);

    //int data[data_size]{1,2,3};
    //std::forward_list<int> ls(3,7);
    std::forward_list<int> ls{};
    //std::forward_list<int> ls(data_size);

    //std::cout << "A ls.max_size(): " << ls.max_size() << "\n";
    for (int i = 0; i < data_size; ++i)
    {
        auto v = distribute(gen);
        ls.push_front(v);
    }
    //std::cout << "B ls.max_size(): " << ls.max_size() << "\n";
    printListWithIterRange("\ncreateRandomList(), ls: ", ls.begin(), ls.end());
    return ls;
}

std::vector<int> createRandomVector(int data_size = 20)
{
    std::random_device rd;
    std::mt19937       gen(rd());
    //std::uniform_real_distribution<double> distribute(0.0, 200.0);
    std::uniform_int_distribution<int> distribute(-100, 200);

    //int data[data_size]{1,2,3};
    //std::forward_list<int> ls(3,7);
    std::vector<int> ls(data_size);
    //std::forward_list<int> ls(data_size);

    //std::cout << "A ls.max_size(): " << ls.max_size() << "\n";
    for (int i = 0; i < data_size; ++i)
    {
        ls[i] = distribute(gen);
        //ls.emplace_back(v);
    }
    //std::cout << "B ls.max_size(): " << ls.max_size() << "\n";
    printListWithIterRange("\ncreateRandomVector(), ls: ", ls.begin(), ls.end());
    return ls;
}

std::forward_list<int> createInvertList(int total)
{
    std::forward_list<int> ls{};

    for (int i = 0; i < total; ++i)
    {
        ls.push_front(i);
    }
    return ls;
}
std::vector<int> createInvertVector(int total)
{
    std::vector<int> ls{};

    for (int i = total - 1; i >= 0; --i)
    {
        ls.emplace_back(i);
    }
    return ls;
}
template <typename Iter>
bool isRandomIter(Iter it)
{
    //
#if __cplusplus > 201402L
    std::cout << "since c++ 17 ...\n";
    if constexpr (std::random_access_iterator<Iter>)
#else
    std::cout << "since c++ 14 ...\n";
    if constexpr (std::_Is_random_iter_v<Iter>)
#endif
    {
        std::cout << "std::_Is_random_iter_v() yes...\n";
        return true;
    }
    else
    {
        std::cout << "std::_Is_random_iter_v() no...\n";
        return false;
    }
}

void testPerformence1(size_t total, int type = 0)
{
    // std::list<int> listA{};

    auto list = createInvertVector(total);
    //auto list = createInvertList(total);
    //auto list = createRandomList();
    //auto list = createRandomVector();
    auto printList = total <= 16;
    if (printList)
    {
        printListWithIterRange("\ninit, list: ", list.begin(), list.end());
    }

    auto time_start = std::chrono::high_resolution_clock::now();
    if (type == 0)
    {
        qsort1(list.begin(), list.end());
    }
    else if (type == 1)
    {
        listQSort(list.begin(), list.end());
        //qsort2(list.begin(), list.end());
    }
    else if (type == 2)
    {
        std::sort(list.begin(), list.end());
    }
    else if (type == 3)
    {
        listQSortFast(list.begin(), list.end());
    }
    //listQSortFast

    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "type: " << type << ", testPerformence1() loss time: " << lossTime << "ms" << std::endl;

    if (printList)
    {
        printListWithIterRange("sort, list: ", list.begin(), list.end());
    }
}
void partSortTest01(size_t total)
{
    auto list      = createInvertVector(total);
    auto a         = std::begin(list);
    auto b         = std::end(list);
    auto value     = *std::next(a, std::distance(a, b) / 2);
    auto printList = total <= 16;
    if (printList)
        printListWithIterRange("\npartSortTest01, a -> b: ", a, b);

    auto time_start     = std::chrono::high_resolution_clock::now();
    auto p0             = std::partition(a, b, [value](const auto& v) -> bool {
        return v < value;
    });
    auto time_end       = std::chrono::high_resolution_clock::now();
    auto lossTime       = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    auto total_lossTime = lossTime;
    std::cout << "std::partition() A loss time: " << lossTime << "ms" << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    //auto p1         = std::partition(p0, b, [value](const auto& v) -> bool {
    //    return v <= value;
    //});
    auto p1  = p0;
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() B loss time: " << lossTime << "ms" << std::endl;
    total_lossTime += lossTime;

    if (printList)
    {
        printListWithIterRange("partSortTest01, a -> p0: ", a, p0);
        printListWithIterRange("partSortTest01, p1 -> b: ", p1, b);
    }
    auto pb = b;
    --pb;
    std::cout << "std::partition() B elements: *a=" << *a << ",*p0=" << *p0 << ",*p1=" << *p1 << ",*pb=" << *pb << std::endl;

    time_start = std::chrono::high_resolution_clock::now();
    std::sort(a, p0);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() AAA loss time: " << lossTime << "ms" << std::endl;
    total_lossTime += lossTime;
    time_start = std::chrono::high_resolution_clock::now();
    std::sort(p1, b);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() BBB loss time: " << lossTime << "ms" << std::endl;
    total_lossTime += lossTime;
    std::cout << "std::partition() BBB          (total / 2): " << (total / 2) << std::endl;
    std::cout << "std::partition() BBB std::distance(a, p0): " << std::distance(a, p0) << std::endl;
    std::cout << "std::partition() BBB std::distance(p1, b): " << std::distance(p1, b) << std::endl;
    // release 3ms,4ms,0ms
    // 多个线程再细分的时候，才会更有效。
    if (printList)
    {
        printListWithIterRange("partSortTest01, list: ", list.begin(), list.end());
    }
    std::cout << "partSortTest01 DDD total_lossTime: " << total_lossTime << "ms" << std::endl;
    // 关于simd排序(4个32bit整形, sse2 __m128i _mm_cmpeq_epi32(__m128i a, __m128i b)): https://www.cita.utoronto.ca/~merz/intel_c10b/main_cls/mergedProjects/intref_cls/common/intref_sse2_int_comparison.htm
    // 还可以用 cuda 并行排序
}

void partSortTest02(size_t total)
{
    auto time_start = std::chrono::high_resolution_clock::now();
    auto list       = createInvertVector(total);
    auto time_end   = std::chrono::high_resolution_clock::now();
    auto lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() build data loss time: " << lossTime << "ms" << std::endl;
    auto total_lossTime = 0;
    auto a              = std::begin(list);
    auto b              = std::end(list);
    auto value          = *std::next(a, std::distance(a, b) / 2);
    auto printList      = total <= 16;
    if (printList)
        printListWithIterRange("\npartSortTest02, a -> b: ", a, b);

    time_start     = std::chrono::high_resolution_clock::now();
    auto p0        = std::partition(a, b, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end       = std::chrono::high_resolution_clock::now();
    lossTime       = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime = lossTime;
    std::cout << "std::partition() A loss time: " << lossTime << "ms" << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    //auto p1         = std::partition(p0, b, [value](const auto& v) -> bool {
    //    return v <= value;
    //});
    auto p1  = p0;
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() B loss time: " << lossTime << "ms" << std::endl;
    total_lossTime += lossTime;

    if (printList)
    {
        printListWithIterRange("partSortTest02, a -> p0: ", a, p0);
        printListWithIterRange("partSortTest02, p1 -> b: ", p1, b);
    }

    value      = *std::next(a, std::distance(a, p0) / 2);
    time_start = std::chrono::high_resolution_clock::now();
    auto s1_p0 = std::partition(a, p0, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end   = std::chrono::high_resolution_clock::now();
    lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() A1 loss time: " << lossTime << "ms" << std::endl;
    auto s1_p1 = s1_p0;

    value      = *std::next(p1, std::distance(p1, b) / 2);
    time_start = std::chrono::high_resolution_clock::now();
    auto s2_p0 = std::partition(p1, b, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end   = std::chrono::high_resolution_clock::now();
    lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() A2 loss time: " << lossTime << "ms" << std::endl;
    auto s2_p1 = s2_p0;

    if (printList)
    {
        printListWithIterRange("std::partition() A elements, a -> p0: ", a, b);
    }
    auto pb = b;
    --pb;
    std::cout << "std::partition() B elements: *a=" << *a << ",*p0=" << *p0 << ",*p1=" << *p1 << ",*pb=" << *pb << std::endl;
    std::cout << "std::partition() B0 elements: *a=" << *a << ",*s1_p0=" << *s1_p0 << ",*s1_p1=" << *s1_p1 << ",*p0=" << *p0 << std::endl;
    std::cout << "std::partition() B1 elements: *p1=" << *p1 << ",*s2_p0=" << *s2_p0 << ",*s2_p1=" << *s2_p1 << ",*pb=" << *pb << std::endl;

    time_start = std::chrono::high_resolution_clock::now();
    std::sort(a, s1_p0);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() AAA_1 0 loss time: " << lossTime << "ms" << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    std::sort(s1_p1, p0);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() AAA_1 1 loss time: " << lossTime << "ms" << std::endl;

    time_start = std::chrono::high_resolution_clock::now();
    std::sort(p1, s2_p0);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() BBB_2 0 loss time: " << lossTime << "ms" << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    std::sort(s2_p1, b);
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() BBB_2 2 loss time: " << lossTime << "ms" << std::endl;

    if (printList)
    {
        printListWithIterRange("partSortTest02, list: ", list.begin(), list.end());
    }
    std::cout << "partSortTest02 DDD total_lossTime: " << total_lossTime << "ms" << std::endl;
}

void partSortTest03(size_t total)
{
    auto time_start = std::chrono::high_resolution_clock::now();
    auto list       = createInvertVector(total);
    auto time_end   = std::chrono::high_resolution_clock::now();
    auto lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() build data loss time: " << lossTime << "ms" << std::endl;
    auto total_lossTime = 0;
    auto a              = std::begin(list);
    auto b              = std::end(list);
    auto value          = *std::next(a, std::distance(a, b) / 2);
    auto printList      = total <= 16;
    if (printList)
        printListWithIterRange("\npartSortTest03, a -> b: ", a, b);

    time_start     = std::chrono::high_resolution_clock::now();
    auto p0        = std::partition(a, b, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end       = std::chrono::high_resolution_clock::now();
    lossTime       = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime = lossTime;
    std::cout << "std::partition() A loss time: " << lossTime << "ms" << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    //auto p1         = std::partition(p0, b, [value](const auto& v) -> bool {
    //    return v <= value;
    //});
    auto p1  = p0;
    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "std::partition() B loss time: " << lossTime << "ms" << std::endl;
    total_lossTime += lossTime;

    if (printList)
    {
        printListWithIterRange("partSortTest03, a -> p0: ", a, p0);
        printListWithIterRange("partSortTest03, p1 -> b: ", p1, b);
    }

    value      = *std::next(a, std::distance(a, p0) / 2);
    time_start = std::chrono::high_resolution_clock::now();
    auto s1_p0 = std::partition(a, p0, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end   = std::chrono::high_resolution_clock::now();
    lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() A1 loss time: " << lossTime << "ms" << std::endl;
    auto s1_p1 = s1_p0;

    value      = *std::next(p1, std::distance(p1, b) / 2);
    time_start = std::chrono::high_resolution_clock::now();
    auto s2_p0 = std::partition(p1, b, [value](const auto& v) -> bool {
        return v < value;
    });
    time_end   = std::chrono::high_resolution_clock::now();
    lossTime   = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "std::partition() A2 loss time: " << lossTime << "ms" << std::endl;
    auto s2_p1 = s2_p0;

    if (printList)
    {
        printListWithIterRange("std::partition() A elements, a -> p0: ", a, b);
    }

    time_start  = std::chrono::high_resolution_clock::now();
    auto sort01 = [&]() -> void {
        std::sort(a, s1_p0);
    };
    auto sort02 = [&]() -> void {
        std::sort(s1_p1, p0);
    };
    auto sort03 = [&]() -> void {
        std::sort(p1, s2_p0);
    };
    auto sort04 = [&]() -> void {
        std::sort(s2_p1, b);
    };

    std::thread thr_sort01(sort01);
    std::thread thr_sort02(sort02);
    std::thread thr_sort03(sort03);
    std::thread thr_sort04(sort04);
    thr_sort01.join();
    thr_sort02.join();
    thr_sort03.join();
    thr_sort04.join();

    time_end = std::chrono::high_resolution_clock::now();
    lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    total_lossTime += lossTime;
    std::cout << "threads sort loss time: " << lossTime << "ms" << std::endl;

    if (printList)
    {
        printListWithIterRange("partSortTest03, list: ", list.begin(), list.end());
    }
    std::cout << "partSortTest03 DDD total_lossTime: " << total_lossTime << "ms" << std::endl;
}
void testMain()
{

    std::cout << "sizeof(long long): " << sizeof(long long) << std::endl;
    std::cout << "sizeof(size_t): " << sizeof(size_t) << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << std::endl;

    std::forward_list<int> ls01{180, 74, 52, -18, 23, -43, 86, -59, 122, -29, 35, -23};
    printListWithIterRange("\nls01: ", ls01.begin(), ls01.end());
    float float_arr01[7]{0.1f, 11.0f, -4.0f, 3.0f, 19.7f, -5.0f, 13.3f};
    printListWithIterRange("\nfloat_arr01: ", &float_arr01[0], &float_arr01[6], "\n\n");
    //return;

    //size_t total = 65536 << 16;// 大约3.9GB, 64bit的64G内存的win10系统无法创建出此数据
    auto total = 65536 << 15; // 此规模的数据同样的系统环境下python无法产生，直接崩溃。小数据的比较计算性能比c++常规排序机制低10%左右
    total        = 65536 << 2;
    //total        = 16;

    auto printList = total <= 16;

    partSortTest01(total);
    std::cout << "\n";
    partSortTest02(total);
    std::cout << "\n";
    partSortTest03(total);
    std::cout << "\n";

    //testPerformence1(total, 0);
    //testPerformence1(total, 1);
    testPerformence1(total, 2);
    //testPerformence1(total, 3);

    std::cout << "\n";
    auto quick_list = createInvertVector(total);
    if (printList)
        printListWithIterRange("\nquick_list init: ", quick_list.begin(), quick_list.end());
    auto time_start = std::chrono::high_resolution_clock::now();
    //quicksort01::quick_sort(quick_list, 0, total - 1);
    //quicksort01::snsort(quick_list, 0, total - 1);
    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    if (printList)
        printListWithIterRange("quick_list sort: ", quick_list.begin(), quick_list.end());

    std::cout << "\nquick_list sort loss time: " << lossTime << "ms" << std::endl;
    std::cout << "\ndata list total: " << total << std::endl;
    return;
    //auto list01 = createRandomList();
    //std::forward_list<int> list01{-5,3,19,6,-8,18,5,21,-7, 7,6};
    std::forward_list<int> list01{180, 74, 52, -18, 117, -66, 169, 191, 70, -36, 161, 104, -23, -43, 86, -59, 122, -29, 35, -23, 180, 74, 52, -18, 117, -66, 169, 191, 70, -36, 161, 104, -23, -43, 86, -59, 122, -29, 35, -23};
    std::cout << "\ninit list01: ";
    for (auto v : list01)
    {
        std::cout << v << ",";
    }
    std::cout << "\n";
    qsort1(list01.begin(), list01.end());
    std::cout << "sorted list01: ";
    for (auto v : list01)
    {
        std::cout << v << ",";
    }
    std::cout << "\n";

    //std::forward_list<int> list02{ -5, 3, 19, 6, -8, 18, 5, 21, -7, 7,6 };
    std::forward_list<int> list02{list01};
    //std::forward_list<int> list02{ 1, 7, -3, 6, 4 };

    //std::forward_list<int> list03(list02);
    //std::vector<int> list03(list02.begin(), list02.end());
    std::vector<int> list03{180, 74, 52, -18, 117, -66, 169, 191, 70, -36, 161, 104, -23, -43, 86, -59, 122, -29, 35, -23, 180, 74, 52, -18, 117, -66, 169, 191, 70, -36, 161, 104, -23, -43, 86, -59, 122, -29, 35, -23};

    //auto it = std::next(list02.begin(), 2);
    //auto isRandomIterBoo = isRandomIter(it);
    //////std::advance(it, 3);
    //////*it = 118;
    ////auto value = *it;
    ////value = 5;
    ////std::cout << "\n*list02.begin(): " << *list02.begin() << std::endl;
    ////auto dis = std::distance(list02.begin(), it);
    ////std::cout << "A dis: " << dis << std::endl;
    //// std::vector���ᱨ��
    //if (isRandomIterBoo) {
    //	auto dis = std::distance(it, std::begin(list02));
    //}
    // vector���ᱨ��
    ////std::cout << "B dis: " << dis << std::endl;
    //std::cout << "test value: " << value << std::endl;
    ////value = 6;
    ////std::cout << "select value: " << value << std::endl;
    //std::cout << "init list02: ";
    //for (auto v : list02)
    //{
    //	std::cout << v << ",";
    //}
    //std::cout << "\n";
    //auto posIT = qpartition(list02.begin(), list02.end(), [value](const auto& v) ->bool {
    //	//std::cout << ">"<< std::endl;
    //	//std::cout << "    >>> v: " << v <<", value: "<< value << std::endl;
    //	//std::cout << "    >>> value: " << value << std::endl;
    //	return v < value;
    //	});
    //std::cout << "qpartition calc list02: ";
    //for (auto v : list02)
    //{
    //	std::cout << v << ",";
    //}
    //std::cout << "\n";

    //std::cout << "*posIT: " << *posIT << std::endl;

    //std::cout << "\ninit list03: ";
    //for (auto v : list03)
    //{
    //	std::cout << v << ",";
    //}
    //std::cout << "\n";
    //qsort2(list03.begin(), list03.end());
    callTimes = 0;
    listQSort(list03.begin(), list03.end());
    std::cout << "sorted list03: ";
    for (auto v : list03)
    {
        std::cout << v << ",";
    }
    std::cout << "\n";
    std::cout << "callTimes: " << callTimes << "\n";
}
} // namespace tsort01
void testMain()
{
    std::cout << "### testMain(), begin ...\n";
    tsort01::testMain();
    std::cout << "### testMain(), end ...\n";
}
} // namespace algorithm::testsort