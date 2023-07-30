#include <iostream>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>
// GLEW
#define GLEW_STATIC
#include <include/GL/glew.h>
// GLFW
#include <include/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/include/stb_image.h"
//
#include "shader.h"

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

std::string toUpperCase(const std::string& src)
{
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(),
                   src.end(),
                   dst.begin(),
                   ::toupper);
    return dst;
}
std::string toLowerCase(const std::string& src)
{
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(),
                   src.end(),
                   dst.begin(),
                   ::tolower);
    return dst;
}


struct TexData
{
    int            width;
    int            height;
    int            nrChannels;
    bool           rgbaEnabled;
    unsigned char* data;
};
std::atomic<bool> asyncLoadFlag(false);

void              asyncLoadRes()
{
    for (auto i = 0; i < 5; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    asyncLoadFlag.store(true, std::memory_order_release);
}
void asyncLoadTexRes(TexData& texData)
{
    //for (auto i = 0; i < 1; ++i)
    //{
    //    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //}

    auto start_time = std::chrono::high_resolution_clock::now();

    auto texPath = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "assets/textures/letterA.png";
    auto imgUrl  = texPath.string();
    auto tempStr = toLowerCase(imgUrl);
    auto rgbaFlag = false;
    if (tempStr.find(".png") != std::string::npos)
    {
        rgbaFlag = true;
    }
    texData.rgbaEnabled = rgbaFlag;
    auto resData             = stbi_load(imgUrl.c_str(), &texData.width, &texData.height, &texData.nrChannels, rgbaFlag ? STBI_rgb_alpha : STBI_rgb);
    texData.data        = resData;
    auto current_time   = std::chrono::high_resolution_clock::now();
    auto elapsedTime    = current_time - start_time;

    std::cout << "asyncLoadTexRes() loss time: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count() << "us\n";

    std::cout << "asyncLoadTexRes(), resData != nullptr: " << (resData != nullptr) << std::endl;
    asyncLoadFlag.store(true, std::memory_order_release);
}

GLuint buildTextures(int width = 128, int height = 128, int nrChannels = 3, unsigned char* resData = nullptr, bool rgbaEnabled = false)
{
    unsigned char* data = resData;
    if (data == nullptr)
    {
        //auto texPath     = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "assets/textures/default.jpg";
        auto texPath = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "assets/textures/letterA.png";
        auto imgUrl  = texPath.string();
        //auto rgbaEnabled = false;
        auto tempStr = toLowerCase(imgUrl);
        std::cout << "toLowerCase(): " << toLowerCase("opengl_Demos") << std::endl;
        std::cout << "toUpperCase(): " << toUpperCase("opengl_Demos") << std::endl;

        if (tempStr.find(".png") != std::string::npos)
        {
            rgbaEnabled = true;
        }
        data = stbi_load(imgUrl.c_str(), &width, &height, &nrChannels, rgbaEnabled ? STBI_rgb_alpha : STBI_rgb);
    }
    else {
        std::cout << "buildTextures(),build texture with erxernal binary data." << std::endl;
    }

    GLuint         texture = GL_ZERO;
    if (data != nullptr)
    {
        std::boolalpha(std::cout);
        std::cout << "buildTextures(), successed to load rgbaEnabled: " << rgbaEnabled << std::endl;
        std::cout << "buildTextures(), successed to load a texture." << std::endl;
        std::cout << "buildTextures(), >>>> tex width:" << width << ", height: " << height << ", nrChannels: " << nrChannels << std::endl;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (rgbaEnabled)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);

        //glPixelStorei(GL_UNPACK_FLIP_Y, this.flipY);
        // deal with odd texture dimensions
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    }
    else
    {
        std::cout << "Failed to load a texture." << std::endl;
    }
    stbi_image_free(data);
    return texture;
}
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(900, 650, "textures", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //o_assert_dbg(1);
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);


    glBlendEquation(GL_FUNC_ADD);
    //glBlendEquation(GL_MIN);
    //glBlendEquation(GL_MAX);
    //glBlendEquation(GL_FUNC_SUBTRACT);
    //glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    //glBlendFunc(GL_ONE, GL_ZERO);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Define the viewport dimensions
    //glViewport(0, 0, 900, 650);

    // Clear the colorbuffer
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    //

    // Create and compile our GLSL program from the shaders

    auto   path_vert       = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "testCases/openglDemos/textures/shader.vert";
    auto   path_vertString = path_vert.string();
    auto   path_frag       = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "testCases/openglDemos/textures/shader.frag";
    auto   path_fragString = path_frag.string();
    GLuint programID       = LoadShaders(path_vertString, path_fragString);


    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    TexData        texData{};

    auto resFlag = false;
    //std::thread resLoadingThr(asyncLoadRes);
    // asyncLoadTexRes(int* width, int* height, int* nrChannels, unsigned char* resData, bool* rgbaEnabled)
    std::thread resLoadingThr(asyncLoadTexRes, std::ref(texData));
    resLoadingThr.detach();

    //auto texture = buildTextures();
    GLuint texture = GL_ZERO;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        if (!resFlag)
        {
            if (asyncLoadFlag.load(std::memory_order_acquire))
            {
                resFlag = true;
                std::cout << "waiting res loaded ...\n";
                std::cout << "loaded tex, width: " << texData.width << ", height: " << texData.height << ", rgbaEnabled: " << texData.rgbaEnabled
                          << "\n";
                texture = buildTextures(texData.width, texData.height, texData.nrChannels, texData.data, texData.rgbaEnabled);
            }
            else
            {
                std::cout << "waiting res loading ...\n";
            }
        }
        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        if (resFlag && texture != GL_ZERO)
        {
            // render
            glBindTexture(GL_TEXTURE_2D, texture);
            glUseProgram(programID);


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Swap the screen buffers
        glfwSwapBuffers(window);
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteProgram(programID);
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    //system("PAUSE");
    return EXIT_SUCCESS;
}