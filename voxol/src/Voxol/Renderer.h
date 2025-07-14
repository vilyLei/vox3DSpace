#ifndef VOXOL_RENDERER_H
#define VOXOL_RENDERER_H

#include "Mat33.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5_webgl.h>

#include <GLES3/gl3.h>

struct ViewportDesc
{
    GLint x = 0;
    GLint y = 0;
    GLint width = 800;
    GLint height = 600;
};
struct CanvasDesc
{
    GLint width = 800;
    GLint height = 600;
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

public:
    static GLuint compileShader(GLenum type, const char *source);
    void startup();
    void setGPUCtxSize(int w, int h);
    void render();

private:
    void init();

private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;
    GLuint program;
    GLuint vao;
    GLint matrixLoc;
    GLint colorLoc;
    ViewportDesc vpDesc{};
    CanvasDesc canvasDesc{};
};

#endif  // VOXOL_RENDERER_H