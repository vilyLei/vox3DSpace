#include <iostream>
#include <cassert>
#include <windows.h>
#include <GL/GL.h>
namespace sample::baseWglCtx02
{
// thanks: https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND             renderingHWnd;
typedef void (*GLFWproc)(void);
typedef PROC(WINAPI* PFN_wglGetProcAddress)(LPCSTR);

typedef const char*(WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);

void* platformLoadModule(const char* path)
{
    return LoadLibraryA(path);
}
PFN_wglGetProcAddress wglGetProcAddress = 0;
PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT = 0;

GLFWproc platformGetModuleSymbol(void* module, const char* name)
{
    return (GLFWproc)GetProcAddress((HMODULE)module, name);
}
int              WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
    HINSTANCE instance;
    instance = (HINSTANCE)platformLoadModule("opengl32.dll");
    if (!instance)
    {
        assert(false, "platformLoadModule() error !!!");
    }
    wglGetProcAddress = (PFN_wglGetProcAddress)
        platformGetModuleSymbol(instance, "wglGetProcAddress");

    if (!wglGetProcAddress)
    {
        assert(false, "platformGetModuleSymbol() error !!!");
    }

    std::string winClassName      = "oglversionchecksample";
    MSG      msg     = {0};
    WNDCLASS wc      = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName              = winClassName.c_str();
    wc.style         = CS_OWNDC;
    if (!RegisterClass(&wc))
        return 1;

    std::wstring winClassWName(winClassName.begin(), winClassName.end());
    renderingHWnd = CreateWindowW(winClassWName.c_str(), L"openglversioncheck", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);



    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);

    return 0;
}
void createWglCtx(HWND hWnd)
{
    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
            PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
            32,                                                         // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8,  // Number of bits for the stencilbuffer
            0,  // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0};

    HGLRC prc, rc;
    HDC   pdc, dc;
    dc = GetDC(hWnd);

    int letWindowsChooseThisPixelFormat;
    letWindowsChooseThisPixelFormat = ChoosePixelFormat(dc, &pfd);
    if (!SetPixelFormat(dc, letWindowsChooseThisPixelFormat, &pfd))
    {
        assert(false, "SetPixelFormat() error !!!");
    }

    rc  = wglCreateContext(dc);
    prc = wglGetCurrentContext();
    if (!rc)
    {
        assert(false, "wglCreateContext() error !!!");
    }
    if (!wglMakeCurrent(dc, rc))
    {
        assert(false, "wglMakeCurrent() error !!!");
        wglMakeCurrent(pdc, prc);
        wglDeleteContext(rc);
    }

    SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    if (!SwapIntervalEXT)
    {
        assert(false, "wglGetProcAddress('wglSwapIntervalEXT') error !!!");
    }

    SwapIntervalEXT(1);
    glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
    glFinish();
    SwapBuffers(dc);
    MessageBoxA(0, (char*)glGetString(GL_VERSION), "baseWglCtx02", 0);
    //wglGetCurrentDC
    //wglMakeCurrent(dc, NULL); Unnecessary; wglDeleteContext will make the context not current
    //wglDeleteContext(rc);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            createWglCtx(hWnd);
            auto autoQuit = true;
            if (autoQuit)
            {
                //PostQuitMessage(0);
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
}