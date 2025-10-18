#ifndef VOXOL_RENDER_OGL_IMAGE_H
#define VOXOL_RENDER_OGL_IMAGE_H

#include "../Base/BaseDefine.h"
#include "OglGpuResUtils.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Render
{
    class OglImage
    {
    public:
        OglImage() = default;
        virtual ~OglImage() = default;
    public:
        void test();
        RawData::Image2DBytesData loadPNGFromAssets(const std::string& filename);
        RawData::Image2DBytesData loadPNG(const std::string& filePath);
    };
}
#endif



//auto texPath = std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "assets/textures/letterA.png";