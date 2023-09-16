
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <bitset>
#include <array>
#include <random>

#include <intrin.h>
#include <windows.h>

namespace threadAffinity
{
std::string getProcessorVendorSerialnumber()
{
    std::array<int, 4> cpuInfo;
    __cpuid(cpuInfo.data(), 1);
    std::ostringstream infoBuf;
    infoBuf
        << std::uppercase << std::hex << std::setfill('0')
        << std::setw(8) << cpuInfo.at(3)
        << std::setw(8) << cpuInfo.at(0);
    return infoBuf.str();
}
std::string getThisThreadIdWithString()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}
void proc(void)
{
    std::random_device rd{};
    std::mt19937       gen(rd());
    std::uniform_int_distribution<unsigned int> distribute(500, 1000);
    using namespace std::chrono_literals;
    std::string          info = "thread id=" + getThisThreadIdWithString() + ", apply cpu ids: ";
    for (auto i = 0; i < 6; ++i)
    {
        //std::this_thread::sleep_for(1s);
        std::this_thread::sleep_for(std::chrono::milliseconds(distribute(gen)));
        //std::string info = "thread id=" + getThisThreadIdWithString() + ",cpuid=" + std::to_string(GetCurrentProcessorNumber()) + ", call.\n";
        //std::cout << info;
        info += std::to_string(GetCurrentProcessorNumber()) + ",";
    }
    info += "\n";
    std::cout << info;
}
void applyThreadAffinity(std::thread& thr, DWORD_PTR inputMask)
{
    std::cout << "SetThreadAffinityMask success  inputMask: " << std::bitset<32>(inputMask) << "\n";
    // thanks: https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadaffinitymask
    // If the function succeeds, the return value is the thread's previous affinity mask.
    // If the function fails, the return value is zero. 
    DWORD_PTR returnMask = SetThreadAffinityMask(thr.native_handle(), inputMask);
    if (returnMask == 0)
    {
        DWORD dwErr = GetLastError();
        std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
    }
    else
    {
        std::cout << "SetThreadAffinityMask success returnMask: " << std::bitset<32>(returnMask) << "\n";
    }
}
void testMain()
{
    std::cout << "threadAffinity::testMain() begin ...\n";
    std::vector<std::thread> threads;

    unsigned int i = 0;

    threads.emplace_back(proc);
    applyThreadAffinity(threads.back(), 0x1);// 0b000001
    i++;
    threads.emplace_back(proc);
    applyThreadAffinity(threads.back(), 0x2);// 0b000010
    i++;
    threads.emplace_back(proc);
    applyThreadAffinity(threads.back(), 0x4);// 0b000100
    i++;
    threads.emplace_back(proc);
    applyThreadAffinity(threads.back(), 0x3);// 0b000011
    i++;

    for (; i < std::thread::hardware_concurrency(); ++i)
    {
        threads.emplace_back(proc);
        applyThreadAffinity(threads.back(), DWORD_PTR(1) << i);
    }

    for (auto& t : threads)
        t.join();
    //
    std::cout << "threadAffinity::testMain() end ...\n";
}
void running(int seconds)
{
    Sleep(seconds * 1000);
    std::cout << "sleep for " << seconds << "(s)" << std::endl;
}

void testMain2()
{
    std::cout << "\nthreadAffinity::testMain2() begin ...\n";
    SetThreadAffinityMask(GetCurrentThread(), 1);
    LARGE_INTEGER start, end;
    LARGE_INTEGER freq;
    //timeConsuming();
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start); //start


    std::cout << "start.QuadPart = " << start.QuadPart << std::endl; //output start
    running(10);                                                     //running 10 seconds
    QueryPerformanceCounter(&end);                                   //end
    std::cout << "end.QuadPart = " << end.QuadPart << std::endl;     //output end

    std::cout << "consume value = end.QuadPart - start.QuadPart = " << (end.QuadPart - start.QuadPart) << std::endl;
    std::cout << "(consume value/(double)freq.QuadPart) Time consumed = " << (end.QuadPart - start.QuadPart) / (double)freq.QuadPart << "(s)" << std::endl; //output consumed time

    std::cout << "threadAffinity::testMain2() end ...\n";
}
}