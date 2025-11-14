#include "OglTestScene.h"
#include "OglText.h"

namespace Voxol::Test
{
void OglTestScene::initScene()
{
    if (entityModeFlag)
    {
        etSysLayers.resize(1);
        for (auto i = 0; i < etSysLayers.size(); i++)
        {
            etSysLayers[i] = System::EntitySystemLayer::make();
        }
        if (etSysLayers.size() > 1)
        {
            etSysLayers[0]->initalize();
            etSysLayers[1]->initalize("scene/IR/scIR02.json");
        }
        else
        {
            //etSysLayers[0]->initalize("scene/IR/scIR04.json");
            etSysLayers[0]->initalize("scene/IR/scIR_interaction.json");
        }

        boundsUnit.color = {0.0f, 0.3, 0.3f, 1.f};
        boundsUnit.vertex.toLine();
        boundsUnit.vertex.lineWidth = 5;
        Render::Gpu::buildBaseDrawUnit(boundsUnit);

        return;
    }
    if (voassModeFlag)
    {
        initVoassScene();
        return;
    }
    baseDrawUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    baseDrawUnit.objMat.setTo(100, 100, 200, 80);
    Render::Gpu::buildBaseDrawUnit(baseDrawUnit);

    texDrawUnit.color = {0.9f, 0.9, 0.9f, 1.0f};
    texDrawUnit.objMat.setTo(150, 70, 100, 220);
    Render::Gpu::buildTexDrawUnit(texDrawUnit);


    static float time = 0.0f;

    time += 0.1f;

    redFormatexDrawUnit.color = {0.2f + std::abs(std::cos(time)) * 0.5f, 0.0, 0.3f, 1.0f};

    redFormatexDrawUnit.objMat.setTo(220, 220, 70, 70);
    Render::Gpu::buildRedFormatTexDrawUnit(redFormatexDrawUnit);

    Render::OglTextGlyphBuilder textBuilder{};
    textBuilder.initFont();
    //auto&& glyData = textBuilder.testBuildGlyph();

    char32_t ch         = U'жа';
    auto&&   glyData    = textBuilder.createGlyph(ch, 32);
    auto&    imgData    = glyData.image;
    glyphDrawUnit.color = {0.0f, 0.3, 0.3f, 1.0f};
    glyphDrawUnit.objMat.setTo(360, 320, imgData.width, imgData.height);

    //Gpu::buildRedFormatTexDrawUnit(glyphDrawUnit, imgData);
    Render::Gpu::buildGlyphTexDrawUnit(glyphDrawUnit, glyData);

    Render::OglImage imgObj{};
    auto&&           pngData = imgObj.loadPNGFromAssets("letterA.png");
    pngUnit.objMat.setTo(360, 150, pngData.width, pngData.height);
    Render::Gpu::buildTexDrawUnit(pngUnit, pngData);

    msdfText.initialize("msdf/arial_atlas.png", "msdf/arial_atlas.json");

    std::string text = "Hello, Cute Boy!";
    msdfText.buildText(text, msdfTextDrawUnits, {300, 100}, 50);
}
void OglTestScene::render(const Voxol::Math::Mat33& vpMat)
{
    //auto& bcp                                        = drawCtx.fboGraph.backgroundClearParam;
    //bcp.viewport   = drawCtx.clearParam.viewport;
    //bcp.clearMask  = drawCtx.clearParam.clearMask;
    //bcp.clearColor = drawCtx.clearParam.clearColor;

    drawCtx.fboGraph.backgroundClearParam = drawCtx.clearParam;
    drawCtx.clearParam.apply(false);
    if (entityModeFlag)
    {
        auto& ctx      = drawCtx;
        auto& params   = ctx.drawParam;
        auto& viewport = ctx.clearParam.viewport;

        for (auto i = 0; i < etSysLayers.size(); i++)
        {
            etSysLayers[i]->updateCtx(ctx);
            etSysLayers[i]->render(vpMat);
        }
        auto  buzy0      = etSysLayers.size() > 1 && !(etSysLayers[1]->uiOpLayer->mouseCtrl.free);
        auto& etSysLayer = buzy0 ? etSysLayers[1] : etSysLayers[0];

        auto& tileSys = etSysLayer->tileSys;
        auto& etSceneSys = etSysLayer->etSceneSys;
        auto& uiOpLayer  = etSysLayer->uiOpLayer;

        etSceneSys->bvh->updateDirty();
        etSceneSys->bvh->endFrameCompact();

        // show mouse picked entity bounds
        boundsUnit.vertex.lineWidth = 1.0f;


        auto bvh = etSceneSys->bvh;
        auto& mouseCtrl   = uiOpLayer->mouseCtrl;

        boundsUnit.color  = {0.0f, 0.3, 0.3f, 1.f};
        auto& queriedEIds = mouseCtrl.qeIds;
        Math::Bounds bounds;
        bounds.toLimit();
        for (auto id : queriedEIds)
        {
            auto& vb = bvh->getBoundsAt(id);
            bounds.expand(vb);
            boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
            boundsUnit.mvp = vpMat;
            boundsUnit.draw();
        }
        boundsUnit.vertex.lineWidth = 5;
        auto vb                     = params.viewWBounds;
        boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
        boundsUnit.mvp = vpMat;
        boundsUnit.draw();
        if (mouseCtrl.isBoundsSelection())
        {
            boundsUnit.color            = {0.6f, 0.0, 0.6f, 1.f};
            boundsUnit.vertex.lineWidth = 1;
            vb                          = mouseCtrl.selectionBounds;
            boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
            boundsUnit.mvp = vpMat;
            boundsUnit.draw();

            boundsUnit.color            = {0.0f, 0.6, 0.6f, 1.f};
            boundsUnit.vertex.lineWidth = 1;
            vb                          = bounds;
            boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
            boundsUnit.mvp = vpMat;
            boundsUnit.draw();            
        }
        return;
    }

    if (voassModeFlag)
    {
        renderVoass(vpMat);
        return;
    }
    baseDrawUnit.mvp = vpMat;
    baseDrawUnit.draw();

    texDrawUnit.mvp = vpMat;
    texDrawUnit.draw();

    redFormatexDrawUnit.mvp = vpMat;
    redFormatexDrawUnit.draw();

    glyphDrawUnit.mvp = vpMat;
    glyphDrawUnit.draw();

    pngUnit.mvp = vpMat;
    pngUnit.draw();

    for (auto& unit : msdfTextDrawUnits)
    {
        unit.mvp = vpMat;
        unit.draw();
    }
}

void OglTestScene::updateKeyboardParams(int key, int scancode, int action, int mods)
{
    etSysLayers[0]->updateKeyboardParams(key, scancode, action, mods);
    if (etSysLayers.size() > 1)
    {
        etSysLayers[1]->updateKeyboardParams(key, scancode, action, mods);
    }
}

void OglTestScene::updateMouseParams(const System::Mouse::MouseInputParam& param)
{
    if (etSysLayers.size() < 2)
    {
        etSysLayers[0]->updateMouseParams(param);
        return;
    }
    auto& etSysLayer1 = etSysLayers[1];
    etSysLayer1->updateMouseParams(param);
    auto& mouseCtrl1 = etSysLayer1->uiOpLayer->mouseCtrl;
    printf("mouseCtrl1.free: %d\n", mouseCtrl1.free);
    if (mouseCtrl1.free)
    {
        etSysLayers[0]->updateMouseParams(param);
    }
    else
    {
        etSysLayers[0]->uiOpLayer->mouseCtrl.free = true;
    }
}
void OglTestScene::initVoassScene()
{
    using namespace Voass::Render;

    auto& sdfCircleUnit       = sdfDrawUnits[0];
    auto& sdfMultiCirclesUnit = sdfDrawUnits[1];
    auto& sdfRingUnit         = sdfDrawUnits[2];
    auto& sdfSectorUnit       = sdfDrawUnits[3];
    auto& sdfRoundedRectUnit  = sdfDrawUnits[4];
    auto& sdfTriangleUnit     = sdfDrawUnits[5];
    auto& sdfRectUnit         = sdfDrawUnits[6];
    auto& strokeShapeUnit     = sdfDrawUnits[7];

    //Gpu::DrawingUnit sdfCircleUnit{};
    bool colorClip      = false;
    sdfCircleUnit.color = {0.9f, 0.0, 0.3f, 1.0f};
    sdfCircleUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle, colorClip);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::MultiCircles);

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle);
    Render::Gpu::buildSDFDrawUnit(sdfMultiCirclesUnit, Shader::SDFShapeType::MultiCircles, colorClip);


    sdfSectorUnit.color = {0.6f, 0.2, 0.5f, 1.f};
    sdfSectorUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfSectorUnit, Shader::SDFShapeType::Sector, colorClip);

    sdfRingUnit.color = {0.6f, 0.2f, 0.6f, 0.5f};
    sdfRingUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfRingUnit, Shader::SDFShapeType::Ring, colorClip);

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfRoundedRectUnit, Shader::SDFShapeType::RoundedRect, colorClip);

    sdfTriangleUnit.color = {0.7f, 0.2, 0.2f, 1.f};
    sdfTriangleUnit.objMat.setTo(300, 350, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfTriangleUnit, Shader::SDFShapeType::Triangle, colorClip);


    sdfRectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    sdfRectUnit.objMat.setTo(100, 100, 200, 80);
    Render::Gpu::buildSDFDrawUnit(sdfRectUnit, Shader::SDFShapeType::Rect, colorClip);

    Render::Gpu::buildSDFDrawUnit(strokeShapeUnit, Shader::SDFShapeType::DefaultShape, colorClip);


    boundsUnit.color = {0.0f, 0.3, 0.3f, 1.f};
    boundsUnit.objMat.setTo(0, 0, 150, 150);
    boundsUnit.vertex.toLine();
    boundsUnit.vertex.lineWidth = 5;
    Render::Gpu::buildBaseDrawUnit(boundsUnit);
}


void OglTestScene::renderVoass(const Math::Mat33& vpMat)
{
    auto& ctx      = drawCtx;
    auto& params   = ctx.drawParam;
    auto& viewport = ctx.clearParam.viewport;

    if (mFbo)
        return;

    mFbo = Render::Draw::OglFbo::make();
    mFbo->init(GL_ZERO);

    auto useTexSampleDrawig = true;
    if (useTexSampleDrawig)
    {
        Render::Draw::ClearParams clearParam{};

        uint32_t  fboSize     = 256 * 2;
        auto fboW        = fboSize;
        auto fboH        = fboSize;
        auto fboTexIndex = 0;

        auto pos     = params.viewMat.getXY();
        auto scaleXY = params.viewMat.getScaleXY();

        Voxol::Math::Mat33 fboVPM{};
        fboVPM.ortho(fboW, fboH);

        Voxol::Math::Mat33 vMat;
        vMat.setXY(pos);
        vMat.setScaleXY(scaleXY);

        fboVPM.append(vMat);

        clearParam.viewport   = {0, 0, fboW, fboH};
        clearParam.clearColor = {0.1, 0.3, 0.1, 1};

        mFbo->bindFBO();
        mFbo->bindTextureAt(tile0Unit.getTextureAt(0), fboTexIndex, fboW, fboH);
        //mFbo.renderBegin({0, 0, fboW, fboH}, {0.1, 0.3, 0.1, 1});
        mFbo->renderBegin(clearParam);

        renderSdfUnits(fboVPM);

        mFbo->unbindFBO(ctx.clearParam);

        Render::Gpu::buildTexDrawUnitWithTex(tile0Unit, mFbo->getTextureAt(fboTexIndex), true);

        fboW = fboH = 256;
        tile0Unit.objMat.setTo(0, 0, fboW, fboH);
        //tile0Unit.mvp = vpMat;
        tile0Unit.mvp = params.projMat;
        tile0Unit.draw();
    }
    else
    {
        renderSdfUnits(vpMat);
    }
}

void OglTestScene::renderSdfUnits(const Voxol::Math::Mat33& vpMat)
{
    auto& sdfCircleUnit       = sdfDrawUnits[0];
    auto& sdfMultiCirclesUnit = sdfDrawUnits[1];
    auto& sdfRingUnit         = sdfDrawUnits[2];
    auto& sdfSectorUnit       = sdfDrawUnits[3];
    auto& sdfRoundedRectUnit  = sdfDrawUnits[4];
    auto& sdfTriangleUnit     = sdfDrawUnits[5];
    auto& sdfRectUnit         = sdfDrawUnits[6];
    auto& strokeShapeUnit     = sdfDrawUnits[7];

    sdfCircleUnit.color = {0.9f, 0.0, 0.7f, 1.0f};
    sdfCircleUnit.mvp   = vpMat;
    sdfCircleUnit.draw();

    sdfCircleUnit.color = {0.7f, 0.7, 0.7f, 1.0f};
    sdfCircleUnit.mvp   = vpMat;
    sdfCircleUnit.draw();

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    sdfMultiCirclesUnit.mvp = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfMultiCirclesUnit.objMat.setTo(360, 130, 200, 200);
    sdfMultiCirclesUnit.color = {0.0, 0.7, 0.7f, 1.0f};
    sdfMultiCirclesUnit.mvp   = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfSectorUnit.mvp = vpMat;
    sdfSectorUnit.draw();

    sdfRingUnit.mvp = vpMat;
    sdfRingUnit.draw();

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    sdfRoundedRectUnit.mvp = vpMat;
    sdfRoundedRectUnit.draw();

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(500, 260, 200, 200);
    sdfRoundedRectUnit.mvp = vpMat;
    sdfRoundedRectUnit.draw();


    sdfRectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    sdfRectUnit.objMat.setTo(100, 300, 300, 30, 3.1415926f / 3);
    sdfRectUnit.mvp = vpMat;
    sdfRectUnit.draw();

    sdfTriangleUnit.setColor(0x8000aa00);
    sdfTriangleUnit.mvp = vpMat;
    sdfTriangleUnit.draw();

    strokeShapeUnit.color = {0.5f, 0.3, 0.3f, 1.0f};
    strokeShapeUnit.objMat.setTo(700, 300, 100, 100);
    strokeShapeUnit.mvp = vpMat;
    strokeShapeUnit.draw();
}
} // namespace Voxol::Test