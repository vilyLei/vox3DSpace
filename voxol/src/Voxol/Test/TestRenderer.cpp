#include <stdio.h>
#include "TestRenderer.h"

#ifdef __EMSCRIPTEN__
namespace Voxol::Test
{
const char* vertShaderSource = R"(#version 300 es
precision highp float;

layout(location = 0) in vec2 a_position;

uniform mat3 u_matrix;

void main() {
    vec3 pos = u_matrix * vec3(a_position, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
})";

const char* fragShaderSource = R"(#version 300 es
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    //outColor = vec4(1.0, 0.8, 0.8, 1.0);
    outColor = u_color;
})";

GLuint TestRenderer::compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

void TestRenderer::startup()
{
    init();
    render();
    // emscripten_set_main_loop(render, 0, 1);
}

void TestRenderer::setMouseXY(float x, float y)
{
    mousePos.x    = x;
    mousePos.y    = y;
    mousePos.flag = true;
}
void TestRenderer::setGPUCtxSize(int w, int h)
{
    vpDesc.width  = w;
    vpDesc.height = h;

    canvasDesc.width  = w;
    canvasDesc.height = h;

    render();
}
void TestRenderer::render()
{
    if (!ctx)
        return;

    glViewport(vpDesc.x, vpDesc.y, vpDesc.width, vpDesc.height);
    // glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindVertexArray(vao);

    auto scale = 0.5f;
    // scale = (std::cos(angle * 3) * 0.5f + 0.5f) * 0.5f + 0.5f;

    Mat33 projM;
    projM.ortho(canvasDesc.width, canvasDesc.height);

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
        auto px = 150;
        auto py = 250;
        if (mousePos.flag)
        {
            px = mousePos.x;
            py = mousePos.y;
        }
        Mat33 objM(px, py, 200, 150);

        // Mat33 mvp = projM * objM;
        Mat33 mvp = objM;
        mvp.prepend(projM);
        ;

        glUniformMatrix3fv(matrixLoc, 1, GL_FALSE, mvp.ptr());
        std::array<float, 4> color = {0.0f, 0.6f, 0.8f, 1.0f};
        glUniform4fv(colorLoc, 1, color.data());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
void TestRenderer::init()
{

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion              = 2;
    attr.enableExtensionsByDefault = true;


    ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

    // 创建 shader program
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragShaderSource);
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
} // namespace Voxol::Test
#endif