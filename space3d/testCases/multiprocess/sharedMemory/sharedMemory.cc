/// License open source MIT
#include <iostream>
#include <tchar.h>
#include "proc/testCreateProcess.h"

#ifdef _WIN32

// #include "windows/SharedMemoryWin.h"
// namespace IPC
// {	typedef SharedMemoryWin SharedMemory;
// }

#else

#ifdef MEM_SYS_V
#include "unix/SharedMemorySysV.h"
namespace IPC
{	typedef SharedMemorySysV SharedMemory;
}
#else
#include "unix/SharedMemoryBSD.h"
namespace IPC
{	typedef SharedMemoryBSD SharedMemory;
}
#endif
#endif
int main(int argc, TCHAR* argv[])
{
    std::cout<<"sharedMemory demo begin ...\n";
    proc::createProcess::testMain(argc, argv);
    return 1;
}
