#ifndef VOXOL_OGL_TEST_SCENE_H
#define VOXOL_OGL_TEST_SCENE_H

#include "../Base/BaseDefine.h"

#include "../Render/OglGpuResUtils.h"
#include "../Math/Mat33.h"
#include "../Math/VxRect.h"
#include "../Render/OglFbo.h"

#include "../System/ShortcutManager.h"
#include "../System/EntitySystemLayer.h"

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

    void updateKeyboardParams(int key, int scancode, int action, int mods);
    void updateMouseParams(const System::Mouse::MouseInputParam& param);

    Render::Draw::DrawContext drawCtx;

private:

    //System::EntitySystemLayer::SP etSysLayer = System::EntitySystemLayer::make();
    //System::EntitySystemLayer::SP etSysLayer1 = System::EntitySystemLayer::make();

    std::vector<System::EntitySystemLayer::SP> etSysLayers;

    void initVoassScene();
    void renderVoass(const Math::Mat33& vpMat);
    void renderSdfUnits(const Voxol::Math::Mat33& vpMat);

    bool voassModeFlag = true;
    bool entityModeFlag = true;

    Render::Draw::OglFbo             mFbo{};

    Render::Gpu::DrawingUnit              baseDrawUnit{};
    Render::Gpu::DrawingUnit              texDrawUnit{};
    Render::Gpu::DrawingUnit              redFormatexDrawUnit{};
    Render::Gpu::DrawingUnit              glyphDrawUnit{};
    Render::Gpu::DrawingUnit              pngUnit{};
    Render::MSDFText                      msdfText{};
    std::vector<Render::Gpu::DrawingUnit>   msdfTextDrawUnits{};

    std::vector<Render::Gpu::DrawingUnit> sdfDrawUnits{8};

    Render::Gpu::DrawingUnit     tile0Unit{};
    Render::Gpu::DrawingUnit boundsUnit{};
};


} // namespace Voxol::Test
#endif // VOXOL_OGL_RENDERER_H
