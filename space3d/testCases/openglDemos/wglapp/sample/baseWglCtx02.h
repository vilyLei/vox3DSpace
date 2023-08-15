#include <iostream>
#include <cassert>
#include <windows.h>
#include <GL/GL.h>
namespace sample::baseWglCtx02
{
// thanks: https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)


typedef int      GLFWbool;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND             renderingHWnd;

typedef HGLRC(WINAPI* PFN_wglCreateContext)(HDC);
typedef BOOL(WINAPI* PFN_wglDeleteContext)(HGLRC);
typedef PROC(WINAPI* PFN_wglGetProcAddress)(LPCSTR);
typedef HDC(WINAPI* PFN_wglGetCurrentDC)(void);
typedef HGLRC(WINAPI* PFN_wglGetCurrentContext)(void);
typedef BOOL(WINAPI* PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL(WINAPI* PFN_wglShareLists)(HGLRC, HGLRC);

typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int);
typedef BOOL(WINAPI* PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, int, int, UINT, const int*, int*);
typedef const char*(WINAPI* PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef const char*(WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

typedef void (*GLFWproc)(void);

void* platformLoadModule(const char* path)
{
    return LoadLibraryA(path);
}

GLFWproc platformGetModuleSymbol(void* module, const char* name)
{
    return (GLFWproc)GetProcAddress((HMODULE)module, name);
}

struct _WGLlibraryModule
{
    HINSTANCE                instance;
    PFN_wglCreateContext     CreateContext;
    PFN_wglDeleteContext     DeleteContext;
    PFN_wglGetProcAddress    GetProcAddress;
    PFN_wglGetCurrentDC      GetCurrentDC;
    PFN_wglGetCurrentContext GetCurrentContext;
    PFN_wglMakeCurrent       MakeCurrent;
    PFN_wglShareLists        ShareLists;

    PFNWGLSWAPINTERVALEXTPROC           SwapIntervalEXT;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
    PFNWGLGETEXTENSIONSSTRINGEXTPROC    GetExtensionsStringEXT;
    PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB;
    GLFWbool                            EXT_swap_control;
    GLFWbool                            EXT_colorspace;
    GLFWbool                            ARB_multisample;
    GLFWbool                            ARB_framebuffer_sRGB;
    GLFWbool                            EXT_framebuffer_sRGB;
    GLFWbool                            ARB_pixel_format;
    GLFWbool                            ARB_create_context;
    GLFWbool                            ARB_create_context_profile;
    GLFWbool                            EXT_create_context_es2_profile;
    GLFWbool                            ARB_create_context_robustness;
    GLFWbool                            ARB_create_context_no_error;
    GLFWbool                            ARB_context_flush_control;
};

_WGLlibraryModule wglmodule;

int winWidth  = 900;
int winHeight = 650;

#ifndef APIENTRY
#    define APIENTRY
#endif
#ifndef APIENTRYP
#    define APIENTRYP APIENTRY*
#endif
#ifndef GLAPI
#    define GLAPI extern
#endif
typedef void(APIENTRYP PFNGLFINISHPROC)(void);
typedef void(APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void(APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void(APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);


#ifndef WGL_ARB_context_flush_control
#    define WGL_ARB_context_flush_control          1
#    define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB       0x2097
#    define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB  0
#    define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
#endif /* WGL_ARB_context_flush_control */

#ifndef WGL_ARB_create_context
#    define WGL_ARB_create_context                 1
#    define WGL_CONTEXT_DEBUG_BIT_ARB              0x00000001
#    define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#    define WGL_CONTEXT_MAJOR_VERSION_ARB          0x2091
#    define WGL_CONTEXT_MINOR_VERSION_ARB          0x2092
#    define WGL_CONTEXT_LAYER_PLANE_ARB            0x2093
#    define WGL_CONTEXT_FLAGS_ARB                  0x2094
#    define ERROR_INVALID_VERSION_ARB              0x2095
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int* attribList);
#    ifdef WGL_WGLEXT_PROTOTYPES
HGLRC WINAPI wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int* attribList);
#    endif
#endif /* WGL_ARB_create_context */

#ifndef WGL_ARB_create_context_profile
#    define WGL_ARB_create_context_profile            1
#    define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#    define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#    define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#    define ERROR_INVALID_PROFILE_ARB                 0x2096
#endif /* WGL_ARB_create_context_profile */

#ifndef WGL_ARB_create_context_robustness
#    define WGL_ARB_create_context_robustness           1
#    define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB           0x00000004
#    define WGL_LOSE_CONTEXT_ON_RESET_ARB               0x8252
#    define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#    define WGL_NO_RESET_NOTIFICATION_ARB               0x8261
#endif /* WGL_ARB_create_context_robustness */

int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
    wglmodule.instance = (HINSTANCE)platformLoadModule("opengl32.dll");
    if (!wglmodule.instance)
    {
        assert(false, "platformLoadModule() error !!!");
    }
    wglmodule.GetProcAddress = (PFN_wglGetProcAddress)
        platformGetModuleSymbol(wglmodule.instance, "wglGetProcAddress");

    if (!wglmodule.GetProcAddress)
    {
        assert(false, "platformGetModuleSymbol() error !!!");
    }

    std::string winClassName      = "baseWglCtxSample";
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
    renderingHWnd = CreateWindowW(winClassWName.c_str(), L"baseWglCtx02", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, winWidth, winHeight, 0, 0, hInstance, 0);

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
    wglmodule.GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
        wglGetProcAddress("wglGetExtensionsStringEXT");
    wglmodule.GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
        wglGetProcAddress("wglGetExtensionsStringARB");
    wglmodule.CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");
    wglmodule.SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    wglmodule.GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
        wglGetProcAddress("wglGetPixelFormatAttribivARB");

    if (!wglmodule.SwapIntervalEXT)
    {
        assert(false, "wglGetProcAddress('wglSwapIntervalEXT') error !!!");
    }
    int contextFlagsGL = 0;
#ifdef _DEBUG
    contextFlagsGL |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

    int contextAttribsGL[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, contextFlagsGL,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0};

    HGLRC hGLRC = wglmodule.CreateContextAttribsARB(dc, NULL, contextAttribsGL);
    //CheckWin32(hGLRC != NULL);

    // Switch to the new context and ditch the old one
    wglMakeCurrent(dc, hGLRC);
    wglDeleteContext(rc);



    //PFNGLCLEARPROC glClear = (PFNGLCLEARPROC)GetProcAddress(hOpenGL32, "glClear");
    auto wgl_glFinish      = (PFNGLFINISHPROC)platformGetModuleSymbol(wglmodule.instance, "glFinish");
    auto wgl_glClear  = (PFNGLCLEARPROC)platformGetModuleSymbol(wglmodule.instance, "glClear");
    auto wgl_glClearColor = (PFNGLCLEARCOLORPROC)GetProcAddress(wglmodule.instance, "glClearColor");
    auto wgl_glViewport    = (PFNGLVIEWPORTPROC)GetProcAddress(wglmodule.instance, "glViewport");
    if (!wgl_glClearColor)
    {
        assert(false, "(PFNGLCLEARPROC)wglmodule.GetProcAddress('glClearColor') error !!!");
    }

    wglmodule.SwapIntervalEXT(1);

    wgl_glViewport(0, 0, winWidth, winHeight);
    wgl_glClearColor(0.5f, 0.0f, 0.5f, 1.0f);
    wgl_glClear(GL_COLOR_BUFFER_BIT);
    wgl_glFinish();
    SwapBuffers(dc);
    //MessageBoxA(0, (char*)glGetString(GL_VERSION), "baseWglCtx02", 0);

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