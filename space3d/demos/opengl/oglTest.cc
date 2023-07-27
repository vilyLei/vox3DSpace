#include <iostream>

// GLEW
#define GLEW_STATIC
#include <include/GL/glew.h>

// GLFW
#include <include/glfw3.h>

#include "player.h"

#define S(x)  SS(x)
#define SS(x) #x

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mousePos_callback(GLFWwindow* window, double posX, double posY);
void mouseButton_callback(GLFWwindow* window, int sign, int flag, int type);
void mouseEnter_callback(GLFWwindow* window, int flag);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

typedef void (*TEST_CALLBACK)(int flag, float value);
//FGAPI void    FGAPIENTRY glutKeyboardFunc(void (*callback)(unsigned char, int, int));
void callbackTest(TEST_CALLBACK cbfunc);

typedef int VOXbool;
#define VOX_TRUE  1
#define VOX_FALSE 0

struct _VOXcontext
{
    int     client;
    int     source;
    int     major;
    int     minor;
    VOXbool forward;
    VOXbool debug;
    VOXbool noerror;
    int     profile;
    int     robustness;
    int     release;
    void*   share;
    struct
    {
        VOXbool offline;
    } nsgl;
};
struct _VOXplatform
{
    int platformID;
    // init
    bool (*init)(void);
};
struct _VOXlibrary
{
    int          initialized;
    void*        instance;
    _VOXplatform platform;
    struct
    {
        void*       framebuffer;
        void*       window;
        _VOXcontext context;
        int         refreshRate;
    } hints;
};
typedef struct _VOXlibrary _VOXlibrary;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    player::OGLPlayer ogl_player;
    ogl_player.showInfo();

    _VOXlibrary _vox = {VOX_FALSE};

    int ver_major = 4;
    int ver_minor = 6;
    std::cout << "opengl version major " << S(ver_major) << std::endl;
    std::cout << "Starting GLFW context, OpenGL " << ver_major << "." << ver_minor << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGLContext", nullptr, nullptr);

    //GLFWimage icons[1];
    //glfwSetWindowIcon(window, 1, icons);
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

    const char* vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* version    = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    std::cout << "vendorName: " << vendorName << std::endl;
    std::cout << "version: " << version << std::endl;
    bool isSupport = glewIsSupported("GL_EXT_framebuffer_object");

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