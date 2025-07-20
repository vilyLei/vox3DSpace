// ..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake cmake ..
// cmake --build .

// ..\..\..\..\dev\webwasm\emsdk\emsdk_env.bat
// emcmake "C:/Program Files/CMake/bin/cmake.exe" -B build -G Ninja
// cmake --build build
//
// http-server -p 666 --cors
/*
glew32d.lib
glfw3.lib
opengl32.lib
kernel32.lib
user32.lib
gdi32.lib
 */

#include <iostream>
#include <cstdint>
#include <cstdio>
#include <chrono>

#ifdef __EMSCRIPTEN__
#    include <emscripten/bind.h>
#    include <emscripten/emscripten.h>
#    include <emscripten/heap.h>
#    include <emscripten/val.h>
#    include <wasm_simd128.h>
#endif


#include "Voxol/Base/SlabArena.h"
#include "Voxol/Base/BaseComponentStorage.h"
#include "Voxol/Base/ComponentWorld.h"
#include "Voxol/Base/EntityIDFactory.h"
#include "Voxol/Base/Component.h"
#include "Voxol/Base/EntityObjectBuilder.h"
#include "Voxol/System/RenderSystem.h"

// #include "Voxol/Render/RenderCmdWorld.h"
#include "Voxol/Motion/RenderCmdWorld.h"
#include "Voxol/Test/TestRenderer.h"

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>
// GLFW    
#include <glfw3.h>

Voxol::Motion::RenderCmdWorld rcmdWorld{};
void                          testMemoryManage()
{
    // printf("testMemoryManage() begin 01...\n");
    // {
    //     using namespace Voxol::Base;
    //     SlabArena arena{};
    //     using TestCompStorage = BaseComponentStorage<Rect, SlabPool<Rect>>;
    //     SlabPool<Rect>  slabPool(arena);
    //     TestCompStorage rectCompStore(std::move(slabPool));
    //     rectCompStore.add(1, {0, 20, 100, 70});
    // }
    // printf("testMemoryManage() end 01...\n");
    printf("testMemoryManage() begin 02...\n");
    {
        using namespace Voxol::Base;
        auto        worldPtr   = EntityObjectBuilder::make();
        VoxolEntity rootEntity = VoxolEntity_None;
        rootEntity             = worldPtr->createEntity("root");
        printf("testMemoryManage() end 02 rootEntity: %d\n", rootEntity);
        auto e01 = worldPtr->createRectFillEntity("rect_01", {15, 25}, {0, 0, 100, 130}, {0xff00aa00}, rootEntity);
        auto e02 = worldPtr->createRectFillGradientBlurEntity("rect_gradient_02", {15, 25}, {0, 0, 100, 130}, {0xff0000aa, 0xffaa0000}, {15}, rootEntity);


        Voxol::System::RenderSystem Renderer;
        Renderer(*worldPtr);

        worldPtr->removeEntity(e01);
    }
    printf("testMemoryManage() end 02...\n");

    printf("testMemoryManage() end ...\n");
}

void calcProfileTest()
{
    auto tot = 10000 * 100;
    printf("calcProfileTest() tot: %d\n", tot);
#ifdef __EMSCRIPTEN__
    double start = emscripten_get_now();
#else
    auto start = std::chrono::high_resolution_clock::now();
#endif

    using namespace Voxol::Math;

    Mat33 mat0(10, 20, 33.5f, -8.2f, 0.3f);
    Mat33 mat1(180, -120, 313.5f, 8.2f, 0.77f);
    Mat33 mat2(90, -120, 313.5f, 8.2f, 10.77f);
    for (auto i = 0; i < tot; i++)
    {
        mat0.prepend(mat1);
        // mat0.append(mat1);
        //mat2 = mat0 * mat1;
    }

#ifdef __EMSCRIPTEN__
    double end      = emscripten_get_now();
    auto   lossTime = end - start;
    printf("calcProfileTest() elapsed time: %fms\n", lossTime);
#else
    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("calcProfileTest() elapsed time: %llums\n", duration.count());
#endif
}



void         key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void         mousePos_callback(GLFWwindow* window, double posX, double posY);
void         mouseButton_callback(GLFWwindow* window, int sign, int flag, int type);
void         mouseEnter_callback(GLFWwindow* window, int flag);
void         scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
const GLuint WIDTH = 800, HEIGHT = 600;

int testOpengl() {
    int ver_major = 4;
    int ver_minor = 6;
    std::cout << "Starting GLFW context, OpenGL " << ver_major << "." << ver_minor << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousePos_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);
    glfwSetCursorEnterCallback(window, mouseEnter_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    //openGL生产商及版本
    const char* vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* version    = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    std::cout << "vendorName： " << vendorName << std::endl;
    std::cout << "version： " << version << std::endl;
    bool isSupport = glewIsSupported("GL_EXT_framebuffer_object"); //是否支持帧缓冲

    int NumberOfExtensions = 0;

    // float k0 = &p0;

    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for (int i = 0; i < NumberOfExtensions; i++)
    {
        const GLubyte* info = glGetStringi(GL_EXTENSIONS, i);
        //Now, do something with ccc
        std::cout << "extends info:" << info << std::endl;
    }
    float* arr = new float[4]{1.0f, 1.1f, 1.2f, 1.3f};
    float  p0  = 0.0f;
    arr[0]     = p0;

    for (auto i = 0; i < 4; ++i)
    {
        std::cout << "arr:" << arr[i] << std::endl;
    }
    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);




        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << "key code: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void mousePos_callback(GLFWwindow* window, double posX, double posY)
{
    std::cout << "mouse pos(" << posX << ", " << posY << ")" << std::endl;
}
void mouseButton_callback(GLFWwindow* window, int sign, int flag, int type)
{
    std::cout << "mouse button( sign=" << sign << ", flag=" << flag << ",type=" << type << ")" << std::endl;
}
void mouseEnter_callback(GLFWwindow* window, int flag)
{

    std::cout << "mouse enter( flag=" << flag << ")" << std::endl;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    std::cout << "mouse button( xoffset=" << xoffset << ", yoffset=" << yoffset << ")" << std::endl;
}
int main()
{
    testMemoryManage();
    calcProfileTest();
    testOpengl();
    return 1;
}
