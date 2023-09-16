#include <iostream>
//#include <vector>
//#include <thread>
//#include <chrono>
//
//#include <windows.h>
//
//void proc(void)
//{
//    using namespace std::chrono_literals;
//    std::this_thread::sleep_for(5s);
//}
#include "threadAffinity.h"

int main()
{
    std::cout << "plthread main begin ...\n";

    threadAffinity::testMain();
    threadAffinity::testMain2();

    //std::vector<std::thread> threads;
    //for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    //{
    //    threads.emplace_back(proc);
    //    DWORD_PTR dw = SetThreadAffinityMask(threads.back().native_handle(), DWORD_PTR(1) << i);
    //    if (dw == 0)
    //    {
    //        DWORD dwErr = GetLastError();
    //        std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
    //    }
    //    else {
    //        std::cout << "SetThreadAffinityMask success dw: " << dw << "...\n";
    //    }
    //}
    //for (auto& t : threads)
    //    t.join();

    std::cout << "plthread main end ...\n";
}