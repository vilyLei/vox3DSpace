#include "OglRenderer.h"

namespace Voxol::Test
{
namespace OglTest
{

const char* vertShaderSource = R"(#version 330 core
precision highp float;

layout(location = 0) in vec2 a_position;

uniform mat3 u_matrix;

void main() {
    vec3 pos = u_matrix * vec3(a_position, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
})";

const char* fragShaderSource = R"(#version 330 core
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    outColor = u_color;
})";

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}
} // namespace OglTest

void         key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void         mousePos_callback(GLFWwindow* window, double posX, double posY);
void         mouseButton_callback(GLFWwindow* window, int sign, int flag, int type);
void         mouseEnter_callback(GLFWwindow* window, int flag);
void         scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


OglRenderer::~OglRenderer()
{
}
int OglRenderer::initCtx()
{
    int ver_major = 3;
    int ver_minor = 3;
    std::cout << "Starting GLFW context, OpenGL " << ver_major << "." << ver_minor << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(ctxWidth, ctxHeight, "LearnOpenGL", nullptr, nullptr);
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

    bool isSupport = glewIsSupported("GL_EXT_framebuffer_object");

    const char* vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const char* version    = reinterpret_cast<const char*>(glGetString(GL_VERSION));

    std::cout << "vendorName: " << vendorName << std::endl;
    std::cout << "version: " << version << std::endl;
    std::cout << "isSupport: " << isSupport << std::endl;

    int NumberOfExtensions = 0;


    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for (int i = 0; i < NumberOfExtensions; i++)
    {
        const GLubyte* info = glGetStringi(GL_EXTENSIONS, i);
        //Now, do something with ccc
        std::cout << "extends info:" << info << std::endl;
    }
    // Define the viewport dimensions
    glViewport(0, 0, ctxWidth, ctxHeight);

    initRender();
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //render();
        draw();

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


void OglRenderer::initRender()
{
    GLuint vs = OglTest::compileShader(GL_VERTEX_SHADER, OglTest::vertShaderSource);
    GLuint fs = OglTest::compileShader(GL_FRAGMENT_SHADER, OglTest::fragShaderSource);
    program   = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    matrixLoc = glGetUniformLocation(program, "u_matrix");
    colorLoc  = glGetUniformLocation(program, "u_color");

    float x = 0, y = 0, w = 1, h = 1;

    float verts[] = {
        x, y,
        x + w, y,
        x, y + h,
        x + w, y + h};

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
}
void OglRenderer::render()
{
    using namespace Voxol::Math;

    if (!program)
        return;

    //glViewport(vpDesc.x, vpDesc.y, vpDesc.width, vpDesc.height);
    //// glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    //glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    auto scale = 0.5f;
    // scale = (std::cos(angle * 3) * 0.5f + 0.5f) * 0.5f + 0.5f;

    Mat33 projM;
    projM.ortho(ctxWidth, ctxHeight);

    {
        Mat33 objM(100, 200, 200, 100);
        // Mat33 mvp = projM * objM;
        Mat33 mvp = projM;
        mvp.append(objM);

        glUniformMatrix3fv(matrixLoc, 1, GL_FALSE, mvp.ptr());
        std::array<float, 4> color = {0.0f, 0.6f, 0.0f, 1.0f};
        glUniform4fv(colorLoc, 1, color.data());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    {
        float px = 150;
        float py = 250;

        Mat33 objM(px, py, 200, 150);

        // Mat33 mvp = projM * objM;
        Mat33 mvp = objM;
        mvp.prepend(projM);

        glUniformMatrix3fv(matrixLoc, 1, GL_FALSE, mvp.ptr());
        std::array<float, 4> color = {0.0f, 0.6f, 0.8f, 1.0f};
        glUniform4fv(colorLoc, 1, color.data());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

void OglRenderer::draw()
{
    using namespace Voxol::Math;

    if (!program)
        return;

    size_t cmdStride = 4;
    size_t cmdIndex = 4;
    size_t   cmdByteIndex = cmdIndex * cmdStride;
    uint32_t cmdsTotal    = 0;

    auto       total = cmdBuf.size();
    const auto ptr   = cmdBuf.data();
    std::memcpy(&cmdsTotal, ptr + cmdByteIndex, sizeof(cmdsTotal));

    cmdIndex ++;

    for (;;)
    {

        uint32_t cmd = 0;
        cmdByteIndex = cmdIndex * cmdStride;
        std::memcpy(&cmd, ptr + cmdByteIndex, sizeof(cmd));
        if (cmd == 0) {
            //printf("cmd to end !!!\n");
            break;
        }
        cmdIndex++;
        cmdByteIndex      = cmdIndex * cmdStride;
        //
        uint32_t descSize = 0;
        std::memcpy(&descSize, ptr + cmdByteIndex, sizeof(cmd));
        switch (cmd)
        {
            case 0x33:
            {
                glUseProgram(program);
                glBindVertexArray(vao);

                cmdByteIndex      = (cmdIndex + 1) * cmdStride;
                uint32_t colorU32 = 0;
                std::memcpy(&colorU32, ptr + cmdByteIndex, sizeof(colorU32));

                auto a = ((colorU32 >> 24) & 0xff) / 255.0f;
                auto r = ((colorU32 >> 16) & 0xff) / 255.0f;
                auto g = ((colorU32 >> 8) & 0xff) / 255.0f;
                auto b = (colorU32 & 0xff) / 255.0f;

                cmdByteIndex  = (cmdIndex + 2) * cmdStride;
                float matvs[9];
                std::memcpy(&matvs, ptr + cmdByteIndex, sizeof(matvs));

                glUniformMatrix3fv(matrixLoc, 1, GL_FALSE, matvs);
                float colorvs[4] = {r,g,b,a};
                glUniform4fv(colorLoc, 1, colorvs);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            }
            default:
                break;
        }
        
        cmdIndex += descSize;

    }
}
void OglRenderer::init()
{
    initCtx();
}
} // namespace Voxol::Test