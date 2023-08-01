/// License open source MIT
#include <iostream>

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
int main()
{
    std::cout<<"sharedMemory demo begin ...\n";
    return 1;
}
