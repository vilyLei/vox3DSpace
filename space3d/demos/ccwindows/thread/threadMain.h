#include <iostream>
#include <algorithm>
#include <variant>
#include <windows.h>
#include <stdio.h>

namespace threadDemo
{
namespace thr_demo_1
{

// thanks: https://www.installsetupconfig.com/win32programming/threadprocesssynchronizationapis11_34.html
// about InterlockedIncrement api: https://learn.microsoft.com/zh-cn/windows/win32/api/winnt/nf-winnt-interlockedincrement64
// Global var

volatile LONG TotalCountOfOuts = 0;



///////////// Thread Main ///////////////////////

void ThreadMain(void)
{

    static DWORD i;

    DWORD dwIncre;

    for (;;)
    {

        wprintf(L"Standard output print, pass # % u\n", i);
        // 32 bit: InterlockedIncrement
        // 64 bit: InterlockedIncrement64
        // 
        // Increments (increases by one) the value of the specified 32-bit

        // variable as an atomic operation.

        // To operate on 64-bit values, use the  InterlockedIncrement64 function.

        //dwIncre = InterlockedIncrement((LPLONG)&TotalCountOfOuts);
        //dwIncre = InterlockedIncrement64((LONG64*)&TotalCountOfOuts);
        dwIncre = InterlockedIncrement64((PLONG64)&TotalCountOfOuts);

        // The function returns the resulting incremented value.

        wprintf(L"Increment value is % u\n", dwIncre);

        Sleep(100);

        i++;
    }
}



///////////////////// Create A Child//////////////

void CreateChildThread(void)
{

    HANDLE hThread;

    DWORD dwId;

    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadMain, (LPVOID)NULL, 0, &dwId);



    if (hThread != NULL)

        wprintf(L"CreateThread() is OK, thread ID % u\n", dwId);

    else

        wprintf(L"CreateThread() failed, error % u\n", GetLastError());



    if (CloseHandle(hThread) != 0)

        wprintf(L"hThread's handle was closed successfully!\n");

    else

        wprintf(L"CloseHandle() failed, error % u\n", GetLastError());
}

/////// Main /////

void testMain(void)
{
    wprintf(L"sizeof(int): % u\n", (int)sizeof(int));
    wprintf(L"sizeof(long): % u\n", (int)sizeof(long));
    wprintf(L"sizeof(long long): % u\n", (int)sizeof(long long));
    wprintf(L"sizeof(WORD): % u\n", (int)sizeof(WORD));
    wprintf(L"sizeof(DWORD): % u\n", (int)sizeof(DWORD));
    wprintf(L"sizeof(LONG64): % u\n", (int)sizeof(LONG64));

    wprintf(L"Current count of the printed lines by child threads = % u\n", TotalCountOfOuts);

    CreateChildThread();

    CreateChildThread();

    auto steps = 10;
    for (;;)
    {
        // 500/100 (from ThreadMain())= 5; Then 5 x 2 threads = 10.
        Sleep(500);
        wprintf(L"Current count of the printed lines by child threads = % u\n", TotalCountOfOuts);
        steps--;
        if (steps < 1)
        {
            break;
        }
    }
}
} // namespace thr_demo_1
void testMain() {
    thr_demo_1::testMain();
}
}
