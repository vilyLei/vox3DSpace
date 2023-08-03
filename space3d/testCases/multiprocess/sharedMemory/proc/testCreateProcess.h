/// License open source MIT
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <chrono>
#include <thread>
#include <sys/stat.h>

namespace proc
{
namespace createProcess
{
inline bool fileExists(const std::string& name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
void testMain(int argc, TCHAR* argv[])
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //if (argc != 2)
    //{
    //    printf("Usage: %s [cmdline]\n", argv[0]);
    //    return;
    //}
    auto cmdns = "./Debug/demo_ccplus.exe";
    cmdns = "./Debug/indexBuffer.exe";
    if (!fileExists(cmdns))
    {
        printf("Error: the cmd exe file does not exists, file path: %s.\n", cmdns);
        return;
    }
    // Start the child process.
    if (CreateProcess(NULL,         // No module name (use command line)
                      (char*)cmdns, //argv[1], // Command line
                      NULL,         // Process handle not inheritable
                      NULL,         // Thread handle not inheritable
                      FALSE,        // Set handle inheritance to FALSE
                      0,            // No creation flags
                      NULL,         // Use parent's environment block
                      NULL,         // Use parent's starting directory
                      &si,          // Pointer to STARTUPINFO structure
                      &pi)          // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess sccess (%s), si size(%dbytes)\n", cmdns, si.cb);
        printf("\tdwProcessId(%d), dwThreadId(%d)\n", pi.dwProcessId, pi.dwThreadId);
    }
    else
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    printf("main process run waiting.\n");
    using namespace std::literals;
    for (auto i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(500ms);
        printf("step i: %d.\n", i);
    }
    printf("main process run waiting with a blocking.\n");
    // Wait until child process exits.
    //WaitForSingleObject(pi.hProcess, INFINITE);
    for (auto i = 0; i < 500000; ++i)
    {
        printf("\twaiting.\n");
        auto waitResult = WaitForSingleObject(pi.hProcess, 800);
        printf("waitResult: 0x%xL.\n", waitResult);
        if (waitResult == WAIT_OBJECT_0)
        {
            break;
        }
    }
    //WaitForSingleObject(pi.hProcess, 0);
    printf("main process run end.\n");

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
}
} // namespace proc