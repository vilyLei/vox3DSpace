#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Tile/TileSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglFbo.h"
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
    void                      initScene();
    void                      render(const Voxol::Math::Mat33& vpMat);
    Render::Draw::DrawContext drawCtx{};

private:
    void initVoassScene();
    void renderVoass(const Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;

    Render::OglFbo             mFbo{};
    Tile::TileSystem           tileSys{};
    Render::EntityRenderSystem etRenderSys{};

    Gpu::DrawingUnit              baseDrawUnit{};
    Gpu::DrawingUnit              texDrawUnit{};
    Gpu::DrawingUnit              redFormatexDrawUnit{};
    Gpu::DrawingUnit              glyphDrawUnit{};
    Gpu::DrawingUnit              pngUnit{};
    MSDFText                      msdfText{};
    std::vector<Gpu::DrawingUnit> msdfTextDrawUnits{};

    std::vector<Gpu::DrawingUnit> sdfDrawUnits{8};

    Gpu::DrawingUnit tile0Unit{};
    Gpu::DrawingUnit boundsUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
