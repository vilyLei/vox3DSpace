#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "OglResUtils.h"
#include "OglImage.h"
#include "OglText.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{

class OglTestScene
{

public:
    OglTestScene()          = default;
    virtual ~OglTestScene() = default;

public:
    void initScene();
    void render(const Voxol::Math::Mat33& projM);


private:

    Gpu::DrawingUnit baseDrawUnit{};
    Gpu::DrawingUnit texDrawUnit{};
    Gpu::DrawingUnit redFormatexDrawUnit{};
    Gpu::DrawingUnit glyphDrawUnit{};
    Gpu::DrawingUnit pngUnit{};
    MSDFText         msdfText{};
    std::vector<Gpu::DrawingUnit> msdfTextDrawUnits{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
