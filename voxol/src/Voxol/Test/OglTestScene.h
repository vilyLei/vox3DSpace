#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Tile/TileSystem.h"
#include "../Render/EntityRenderSystem.h"
#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglFbo.h"
#include "../System/MouseEventSystem.h"
#include "../Render/OglGpuResUtils.h"
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
    void                      setKeyParams(int key, int scancode, int action, int mode);
    void                      setMouseParams(const System::UIMouseParam& param);
    Render::Draw::DrawContext drawCtx{};
    System::Mouse::EventManager       mouseEvtMana{};

private:
    void initVoassScene();
    void renderVoass(const Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;
    bool entityModeFlag = true;

    Render::OglFbo             mFbo{};
    Tile::TileSystem::SP       tileSys = Tile::TileSystem::make();
    Render::EntityRenderSystem::SP etRenderSys = Render::EntityRenderSystem::make();

    Render::Gpu::DrawingUnit              baseDrawUnit{};
    Render::Gpu::DrawingUnit              texDrawUnit{};
    Render::Gpu::DrawingUnit              redFormatexDrawUnit{};
    Render::Gpu::DrawingUnit              glyphDrawUnit{};
    Render::Gpu::DrawingUnit              pngUnit{};
    Render::MSDFText                      msdfText{};
    std::vector<Render::Gpu::DrawingUnit>   msdfTextDrawUnits{};

    std::vector<Render::Gpu::DrawingUnit> sdfDrawUnits{8};

    Render::Gpu::DrawingUnit     tile0Unit{};
    Render::Gpu::DrawingUnit     boundsUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
