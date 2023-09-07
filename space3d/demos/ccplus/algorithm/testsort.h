#pragma once

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <vector>
#include <functional>
#include <iterator>
#include <random>

namespace algorithm::testsort
{
namespace tsort01
{
int callTimes = 0;


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
std::forward_list<int> createRandomList()
{
    std::random_device rd;
    std::mt19937       gen(rd());
    //std::uniform_real_distribution<double> distribute(0.0, 200.0);
    std::uniform_int_distribution<int> distribute(-100, 200);

    const int data_size = 20;
    //int data[data_size]{1,2,3};
    //std::forward_list<int> ls(3,7);
    std::forward_list<int> ls{};
    //std::forward_list<int> ls(data_size);

    std::cout << "A ls.max_size(): " << ls.max_size() << "\n";
    for (int i = 0; i < data_size; ++i)
    {
        auto v = distribute(gen);
        ls.push_front(v);
    }
    std::cout << "B ls.max_size(): " << ls.max_size() << "\n";
    std::cout << "\nls: ";
    for (auto v : ls)
    {
        std::cout << v << ",";
    }
    std::cout << "\n";
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

void testPerformence1(int total, int type = 0)
{
    auto list = createInvertVector(total);
    //auto list = createInvertList(total);
    //auto list = createRandomList();

    if (total < 16)
    {
        std::cout << "\ninit list01: ";

        for (auto v : list)
        {
            std::cout << v << ",";
        }
        std::cout << "\n";
    }

    auto time_start = std::chrono::high_resolution_clock::now();
    if (type  < 1)
    {
        qsort1(list.begin(), list.end());
    }
    else if (type == 1)
    {
        listQSort(list.begin(), list.end());
        //qsort2(list.begin(), list.end());
    }
    else {
        std::sort(list.begin(), list.end());
    }

    auto time_end = std::chrono::high_resolution_clock::now();
    auto lossTime = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
    std::cout << "type: " << type << ", testPerformence1() loss time: " << lossTime << "ms" << std::endl;

    if (total < 16)
    {
        std::cout << "sorted list01: ";
        for (auto v : list)
        {
            std::cout << v << ",";
        }
        std::cout << "\n";
    }
}
void testMain()
{
    auto total = 65535 << 4;
    testPerformence1(total, 0);
    testPerformence1(total, 1);
    testPerformence1(total, 2);
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