#pragma once

#include "quad.h"
#include "texture.h"

class Renderer
{
private:
    //unsigned int width;
    //unsigned int height;
    UVec2 resolution;

    Texture       texture;
    ComputeShader paintTexture;
    Pipeline      paintTexturePipeline;

    Quad           quad;
    VertexShader   vertexShader;
    FragmentShader fragmentShader;
    Pipeline       renderPipeline;

public:
    Renderer(unsigned int pw, unsigned ph) :
        resolution {pw, ph},
        texture{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT},
        paintTexture(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "demos/opengl_compute/texv.comp"),
        vertexShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "demos/opengl_compute/texv.vert"),
        fragmentShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                       "demos/opengl_compute/texv.frag")
    {
        paintTexturePipeline.attachComputeShader(paintTexture);

        renderPipeline.attachVertexShader(vertexShader);
        renderPipeline.attachFragmentShader(fragmentShader);

        std::cout << "Renderer::construct()... "<< std::endl;
        std::cout << "resolution: " << resolution.toString() << std::endl;
    }
    ~Renderer()
    {
        std::cout << "Renderer::deconstruct() ..." << std::endl;
    }
    void setResolution(const UVec2& resolution)
    {
        this->resolution = resolution;
        texture.resize(resolution);
    }

    void render() const
    {
        // run compute shader
        texture.bindToImageUnit(0, GL_WRITE_ONLY);
        paintTexturePipeline.activate();
        glDispatchCompute(resolution.x, resolution.y, 1);
        paintTexturePipeline.deactivate();

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // render quad
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, resolution.x, resolution.y);
        texture.bindToTextureUnit(0);
        quad.draw(renderPipeline);
    }
};