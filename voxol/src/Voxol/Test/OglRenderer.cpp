#include "OglRenderer.h"
#include <chrono>

namespace Voxol::Test
{

GLuint OglRenderer::ctxCurrWidth = 1200;
GLuint OglRenderer::ctxCurrHeight = 800;

void OglRenderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << "key code: " << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
void OglRenderer::mouseEnter_callback(GLFWwindow* window, int flag)
{

    std::cout << "mouse enter(flag=" << flag << ")" << std::endl;
}
void OglRenderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    int fw;
    int fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    ctxCurrWidth  = fw;
    ctxCurrHeight = fh;
    auto renderer = static_cast<OglRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        renderer->dirty = true;
    }
}

void OglRenderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //std::cout << "mouse button( xoffset=" << xoffset << ", yoffset=" << yoffset << ")" << std::endl;
    auto renderer = static_cast<OglRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        auto  mouseActType = 4;
        auto& mousePos     = renderer->mousePos;
        Voxol::Motion::UIMouseParam param{mousePos.x, mousePos.y, mouseActType, yoffset};
        //renderer->setMouseParams(mousePos.x, mousePos.y, mouseActType, yoffset);
        renderer->setMouseParams(param);
    }
}
void OglRenderer::mousePos_callback(GLFWwindow* window, double posX, double posY)
{
    //std::cout << "OglRenderer::mousePos_callback(), mouse pos(" << posX << ", " << posY << ")" << std::endl;
    auto renderer = static_cast<OglRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        renderer->setMouseXY(posX, posY);
    }
}

void OglRenderer::mouseButton_callback(GLFWwindow* window, int sign, int flag, int type)
{
    //std::cout << "mouse button( sign=" << sign << ", flag=" << flag << ",type=" << type << ")" << std::endl;
    auto renderer = static_cast<OglRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer)
    {
        auto  btn          = sign + 1;
        auto  mouseActType = flag > 0 ? btn * 10 + 1 : btn * 10 + 2;
        auto& mousePos     = renderer->mousePos;
        //renderer->setMouseParams(mousePos.x, mousePos.y, mouseActType, 0);
        Voxol::Motion::UIMouseParam param{mousePos.x, mousePos.y, mouseActType, 0};
        renderer->setMouseParams(param);
    }
}
int OglRenderer::initCtx()
{
    int ver_major = 3;
    int ver_minor = 3;
    std::cout << "Starting GLFW context, OpenGL " << ver_major << "." << ver_minor << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glDisable(GL_MULTISAMPLE);
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(ctxCurrWidth, ctxCurrHeight, "VoxolModule", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousePos_callback);
    glfwSetMouseButtonCallback(window, mouseButton_callback);
    glfwSetCursorEnterCallback(window, mouseEnter_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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


    //glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    //for (int i = 0; i < NumberOfExtensions; i++)
    //{
    //    const GLubyte* info = glGetStringi(GL_EXTENSIONS, i);
    //    std::cout << "extends info:" << info << std::endl;
    //}

    int fw;
    int fh;
    glfwGetFramebufferSize(window, &fw, &fh);
    ctxCurrWidth = fw;
    ctxCurrHeight = fh;

    initRenderRes();
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        if (dirty) {

            glViewport(0, 0, ctxCurrWidth, ctxCurrHeight);
            // Render
            // Clear the colorbuffer
            glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            render();
            // draw();

            // Swap the screen buffers
            glfwSwapBuffers(window);
        }
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void OglRenderer::setMouseXY(float x, float y)
{
    mousePos             = {x, y};
    if (!mousePos.isEqual(canvas.view.mousePos))
    {
        canvas.view.mousePos = mousePos;
        setMouseParams({mousePos.x, mousePos.y, 3, 0});
    }
}

void OglRenderer::setMouseParams(const Voxol::Motion::UIMouseParam& param)
{
    auto flag = mouseCtrl.setMouseParams(canvas.view, param);
    dirty     = dirty || flag;
}


void OglRenderer::initRenderRes()
{
    mScene.initScene();
}
void OglRenderer::render()
{
    using namespace Voxol::Math;

    if (ctxWidth != ctxCurrWidth || ctxHeight != ctxCurrHeight)
    {
        ctxWidth = ctxCurrWidth;
        ctxHeight = ctxCurrHeight;
        canvas.view.projMat.ortho(ctxWidth, ctxHeight);
        dirty = true;
    }
    //if (dirty)
    //{
        Mat33 mat = canvas.view.projMat;
        mat.append(canvas.view.viewMat);
        mScene.ctxCurrWidth = ctxCurrWidth;
        mScene.ctxCurrHeight = ctxCurrHeight;
        mScene.projMat = canvas.view.projMat;
        mScene.viewMat = canvas.view.viewMat;
        mScene.render(mat);
    //}

    dirty = false;
}

void OglRenderer::draw()
{
    /*
    using namespace Voxol::Math;

    if (!program)
        return;

    if (onDraw) {
        onDraw(ctxWidth, ctxHeight);
    }
    if (cmdBuf.empty())
        return;

    size_t cmdStride = 4;
    size_t cmdIndex = 3;
    size_t   cmdByteIndex = cmdIndex * cmdStride;
    uint32_t cmdsTotal    = 0;

    auto       total = cmdBuf.size();
    const auto ptr   = cmdBuf.data();
    std::memcpy(&cmdsTotal, ptr + cmdByteIndex, sizeof(cmdsTotal));

    cmdIndex++;

    for (;;)
    {
        uint32_t cmd = 0;
        cmdByteIndex = cmdIndex * cmdStride;
        std::memcpy(&cmd, ptr + cmdByteIndex, sizeof(cmd));
        if (cmd == 0) {
            //printf("cmd to end !!!\n");
            break;
        }
        cmdByteIndex      = (cmdIndex +1) * cmdStride;

        uint32_t descSize = 0;
        std::memcpy(&descSize, ptr + cmdByteIndex, sizeof(cmd));
        switch (cmd)
        {
            case 0x33:
            {
                glUseProgram(program);
                glBindVertexArray(vao);

                cmdByteIndex      = (cmdIndex + 2) * cmdStride;
                uint32_t colorU32 = 0;
                std::memcpy(&colorU32, ptr + cmdByteIndex, sizeof(colorU32));

                auto a = ((colorU32 >> 24) & 0xff) / 255.0f;
                auto r = ((colorU32 >> 16) & 0xff) / 255.0f;
                auto g = ((colorU32 >> 8) & 0xff) / 255.0f;
                auto b = (colorU32 & 0xff) / 255.0f;

                cmdByteIndex  = (cmdIndex + 3) * cmdStride;
                float matvs[9];
                std::memcpy(&matvs, ptr + cmdByteIndex, sizeof(matvs));

                glUniformMatrix3fv(matrixLoc, 1, GL_FALSE, matvs);
                float colorvs[4] = {r,g,b,a};
                glUniform4fv(colorLoc, 1, colorvs);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            }
            break;
            default:
            {
                descSize = 0;
            }
                break;
        }
        if (descSize < 1) {
            break;
        }
        cmdIndex += descSize;

    }
    //*/
}
void OglRenderer::init()
{
    initCtx();
}
} // namespace Voxol::Test