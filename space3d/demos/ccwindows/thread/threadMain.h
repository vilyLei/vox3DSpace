#include <iostream>
#include <algorithm>
#include <variant>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

namespace threadDemo
{
namespace thr_demo_2
{
// thanks: https://learn.microsoft.com/en-us/windows/win32/procthread/creating-threads

#define MAX_THREADS 3
#define BUF_SIZE    255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void         ErrorHandler(LPCTSTR lpszFunction);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData
{
    int val1;
    int val2;
} MYDATA, *PMYDATA;


int testMain()
{
    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    // Create MAX_THREADS worker threads.

    for (int i = 0; i < MAX_THREADS; i++)
    {
        // Allocate memory for thread data.

        pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                           sizeof(MYDATA));

        if (pDataArray[i] == NULL)
        {
            // If the array allocation fails, the system is out of memory
            // so there is no point in trying to print an error message.
            // Just terminate execution.
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.

        pDataArray[i]->val1 = i;
        pDataArray[i]->val2 = i + 100;

        // Create the thread to begin execution on its own.

        hThreadArray[i] = CreateThread(
            NULL,                 // default security attributes
            0,                    // use default stack size
            MyThreadFunction,     // thread function name
            pDataArray[i],        // argument to thread function
            0,                    // use default creation flags
            &dwThreadIdArray[i]); // returns the thread identifier


        // Check the return value for success.
        // If CreateThread fails, terminate execution.
        // This will automatically clean up threads and memory.

        if (hThreadArray[i] == NULL)
        {
            ErrorHandler(TEXT("CreateThread"));
            ExitProcess(3);
        }
    } // End of main thread creation loop.

    // Wait until all threads have terminated.

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    // Close all thread handles and free memory allocations.

    for (int i = 0; i < MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if (pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL; // Ensure address is not reused.
        }
    }

    return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    HANDLE  hStdout;
    PMYDATA pDataArray;

    TCHAR  msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD  dwChars;

    // Make sure there is a console to receive output results.

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because
    // it was checked for NULL before the thread was created.

    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"),
                    pDataArray->val1, pDataArray->val2);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    return 0;
}



void ErrorHandler(LPCTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD  dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
}
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
        FILETIME unused;
        FILETIME utime;
        auto hThread = GetCurrentThread();
        auto hThreadId = GetCurrentThreadId();
        auto     thrTimes  = GetThreadTimes(hThread, &unused, &unused, &unused, &utime);
        wprintf(L"Standard output print, pass # % u\n, 0xhThread:%x, threadId:%x, ", i, hThread, hThreadId);
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

HANDLE CreateChildThread(bool closedHandle)
{

    HANDLE hThread = NULL;
    DWORD dwId;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadMain, (LPVOID)NULL, 0, &dwId);

    if (hThread != NULL)
        wprintf(L"CreateThread() is OK, thread ID % u\n", dwId);
    else
        wprintf(L"CreateThread() failed, error % u\n", GetLastError());

    wprintf(L"CreateThread() hThread A: 0x%x\n", hThread);
    if (closedHandle)
    {
        if (hThread != NULL && CloseHandle(hThread) != 0)
            wprintf(L"hThread's handle was closed successfully!\n");
        else
            wprintf(L"CloseHandle() failed, error % u\n", GetLastError());
        //CloseHandle(hThread);
    }
    wprintf(L"CreateThread() hThread B: 0x%x\n", hThread);
    return hThread;
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

    bool closedHandle = true;

    auto thr0 = CreateChildThread(closedHandle);
    auto thr1 = CreateChildThread(closedHandle);

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
    if (!closedHandle)
    {
        if (thr0 != NULL)
        {
            CloseHandle(thr0);
        }
        if (thr1 != NULL)
        {
            CloseHandle(thr1);
        }
    }
}
} // namespace thr_demo_1
void testMain()
{
    //PULONG_PTR LowLimit = 0;
    //PULONG_PTR HighLimit = 0;
    //GetCurrentThreadStackLimits(LowLimit, HighLimit);
    

    //thr_demo_1::testMain();
    thr_demo_2::testMain();
}
} // namespace threadDemo
