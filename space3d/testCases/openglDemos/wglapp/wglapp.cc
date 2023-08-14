#include <iostream>
#include <windows.h>
#include "sample/baseWglCtx.h"
#include "sample/baseWglCtx02.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    //sample::baseWglCtx::WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    sample::baseWglCtx02::WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    return 1;
}