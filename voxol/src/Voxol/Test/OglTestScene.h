#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "OglResUtils.h"
#include "OglImage.h"
#include "OglText.h"
#include "OglFbo.h"

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
    void render(const Voxol::Math::Mat33& vpMat);

    Voxol::Math::Mat33 projMat{};
    Voxol::Math::Mat33 viewMat{};

private:

    
    void initVoassScene();
    void renderVoass(const Voxol::Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;

    OglFbo mFbo{};

    Gpu::DrawingUnit baseDrawUnit{};
    Gpu::DrawingUnit texDrawUnit{};
    Gpu::DrawingUnit redFormatexDrawUnit{};
    Gpu::DrawingUnit glyphDrawUnit{};
    Gpu::DrawingUnit pngUnit{};
    MSDFText         msdfText{};
    std::vector<Gpu::DrawingUnit> msdfTextDrawUnits{};

    
    Gpu::DrawingUnit sdfCircleUnit{};
    Gpu::DrawingUnit sdfMultiCirclesUnit{};
    Gpu::DrawingUnit sdfRingUnit{};
    Gpu::DrawingUnit sdfSectorUnit{};
    Gpu::DrawingUnit sdfRoundedRectUnit{};
    Gpu::DrawingUnit sdfTriangleUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
