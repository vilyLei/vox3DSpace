#include <iostream>
#include <windows.h>
#include <GL/GL.h>
namespace sample::baseWglCtx
{
// thanks: https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
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
    CreateWindowW(winClassWName.c_str(), L"openglversioncheck", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // PFD: PIXELFORMATDESCRIPTOR, location: https://learn.microsoft.com/zh-cn/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
    switch (message)
    {
        case WM_CREATE:
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

            HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

            int letWindowsChooseThisPixelFormat;
            letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
            SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

            HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
            wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

            MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
            //wglGetCurrentDC
            //wglMakeCurrent(ourWindowHandleToDeviceContext, NULL); Unnecessary; wglDeleteContext will make the context not current
            wglDeleteContext(ourOpenGLRenderingContext);
            PostQuitMessage(0);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
}