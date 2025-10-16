#include "OglTestScene.h"
#include "OglText.h"

namespace Voxol::Test
{


void OglTestScene::initScene()
{
    if (voassModeFlag)
    {
        initVoassScene();
        return;
    }
    baseDrawUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    baseDrawUnit.objMat.setTo(100, 100, 200, 80);
    Gpu::buildBaseDrawUnit(baseDrawUnit);

    texDrawUnit.color = {0.9f, 0.9, 0.9f, 1.0f};
    texDrawUnit.objMat.setTo(150, 70, 100, 220);
    Gpu::buildTexDrawUnit(texDrawUnit);


    static float time        = 0.0f;

    time += 0.1f;

    redFormatexDrawUnit.color = {0.2f + std::abs(std::cos(time)) * 0.5f, 0.0, 0.3f, 1.0f};

    redFormatexDrawUnit.objMat.setTo(220, 220, 70, 70);
    Gpu::buildRedFormatTexDrawUnit(redFormatexDrawUnit);

    OglTextGlyphBuilder textBuilder{};
    textBuilder.initFont();
    //auto&& glyData = textBuilder.testBuildGlyph();

    char32_t ch         = U'жа';
    auto&&   glyData    = textBuilder.createGlyph(ch, 32);
    auto&   imgData = glyData.image;
    glyphDrawUnit.color = {0.0f, 0.3, 0.3f, 1.0f};
    glyphDrawUnit.objMat.setTo(360, 320, imgData.width, imgData.height);

    //Gpu::buildRedFormatTexDrawUnit(glyphDrawUnit, imgData);
    Gpu::buildGlyphTexDrawUnit(glyphDrawUnit, glyData);

    OglImage imgObj{};
    auto&&   pngData = imgObj.loadPNGFromAssets("letterA.png");
    pngUnit.objMat.setTo(360, 150, pngData.width, pngData.height);
    Gpu::buildTexDrawUnit(pngUnit, pngData);

    msdfText.initialize("msdf/arial_atlas.png", "msdf/arial_atlas.json");

    std::string text = "Hello, Cute Boy!";
    msdfText.buildText(text, msdfTextDrawUnits, {300, 100}, 50);
    
}
void OglTestScene::render(const Voxol::Math::Mat33& vpMat)
{
    drawCtx.clearParam.apply();

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

void OglTestScene::setMouseParams(const Voxol::Motion::UIMouseParam& param)
{
    //auto& ctx    = drawCtx;
    //auto& drawParam = ctx.drawParam;
    //Math::Vec2 wpv = drawParam.invViewMat.mapPoint({param.x, param.y});
    mouseEvtMana.upateMouseParam(drawCtx, etRenderSys.bvh, etRenderSys.storage, param);
}
void  OglTestScene::initVoassScene(){

    
    using namespace Voass::Render;

    tileSys.initalize();
    etRenderSys.initalize();

    auto& sdfCircleUnit      = sdfDrawUnits[0];
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
    Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle, colorClip);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::MultiCircles);

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    //Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle);
    Gpu::buildSDFDrawUnit(sdfMultiCirclesUnit, Shader::SDFShapeType::MultiCircles, colorClip);


    sdfSectorUnit.color = {0.6f, 0.2, 0.5f, 1.f};
    sdfSectorUnit.objMat.setTo(100, 100, 200, 200);
    Gpu::buildSDFDrawUnit(sdfSectorUnit, Shader::SDFShapeType::Sector, colorClip);

    sdfRingUnit.color = {0.6f, 0.2f, 0.6f, 0.5f};
    sdfRingUnit.objMat.setTo(100, 100, 200, 200);
    Gpu::buildSDFDrawUnit(sdfRingUnit, Shader::SDFShapeType::Ring, colorClip);

    sdfRoundedRectUnit.color = {0.5f, 0.6, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    Gpu::buildSDFDrawUnit(sdfRoundedRectUnit, Shader::SDFShapeType::RoundedRect, colorClip);

    sdfTriangleUnit.color = {0.7f, 0.2, 0.2f, 1.f};
    sdfTriangleUnit.objMat.setTo(300, 350, 200, 200);
    Gpu::buildSDFDrawUnit(sdfTriangleUnit, Shader::SDFShapeType::Triangle, colorClip);
    
    
    sdfRectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    sdfRectUnit.objMat.setTo(100, 100, 200, 80);
    Gpu::buildSDFDrawUnit(sdfRectUnit, Shader::SDFShapeType::Rect, colorClip);

    Gpu::buildSDFDrawUnit(strokeShapeUnit, Shader::SDFShapeType::DefaultShape, colorClip);

    
    boundsUnit.color = {0.0f, 0.3, 0.3f, 1.f};
    boundsUnit.objMat.setTo(0, 0, 150, 150);
    boundsUnit.vertex.toLine();
    boundsUnit.vertex.lineWidth = 5;
    Test::Gpu::buildBaseDrawUnit(boundsUnit);
}


void OglTestScene::renderVoass(const Math::Mat33& vpMat)
{
    auto& ctx    = drawCtx;
    auto& params   = ctx.drawParam;
    auto& viewport = ctx.clearParam.viewport;
    //etRenderSys

    auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) {
        etRenderSys.render(drawCtx, vpMat, sdfDrawUnits);
    };
    auto queryCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) -> int {
        return etRenderSys.drawQuery(bounds, vpMat);
    };

    /*
    auto drawCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) {
        renderSdfUnits(vpMat);
    };
    auto queryCall = [this](const Math::VxRect& bounds, const Math::Mat33& vpMat) -> int {
        return 1;
    };
    //*/
    //drawCall({} , vpMat);
    //return;
    ctx.drawCall  = drawCall;
    ctx.drawQuery = queryCall;
    tileSys.run(ctx);

    // show mouse picked entity bounds
    boundsUnit.vertex.lineWidth = 1.0f;

    auto& queriedEIds = mouseEvtMana.queryEIds;

    for (auto id : queriedEIds)
    {
        //auto& vb = etRenderSys.bvhItems[id].bounds;
        auto& vb = etRenderSys.bvh.getBoundsAt(id);
        boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
        boundsUnit.mvp = vpMat;
        boundsUnit.draw();

    }
    boundsUnit.vertex.lineWidth = 5;
    auto vb = params.viewWBounds;
    boundsUnit.objMat.setTo(vb.x(), vb.y(), vb.width(), vb.height());
    boundsUnit.mvp = vpMat;
    boundsUnit.draw();
    return;

    mFbo.init(GL_ZERO);

    auto useTexSampleDrawig = true;
    if (useTexSampleDrawig)
    {
        Render::Draw::ClearParams clearParam{};

        int  fboSize     = 256 * 2;
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

        clearParam.viewport = {0, 0, fboW, fboH};
        clearParam.clearColor = {0.1, 0.3, 0.1, 1};

        mFbo.bindFBO();
        mFbo.bindTextureAt(tile0Unit.getTextureAt(0), fboTexIndex, fboW, fboH);
        //mFbo.renderBegin({0, 0, fboW, fboH}, {0.1, 0.3, 0.1, 1});
        mFbo.renderBegin(clearParam);
                
        renderSdfUnits(fboVPM);

        mFbo.unbindFBO(ctx.clearParam);

        Gpu::buildTexDrawUnitWithTex(tile0Unit, mFbo.getTextureAt(fboTexIndex), true);

        fboW = fboH = 256;
        tile0Unit.objMat.setTo(0, 0, fboW, fboH);
        //tile0Unit.mvp = vpMat;
        tile0Unit.mvp = params.projMat;
        tile0Unit.draw();
    }
    else {
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
    sdfCircleUnit.mvp = vpMat;
    sdfCircleUnit.draw();

    sdfCircleUnit.color = {0.7f, 0.7, 0.7f, 1.0f};
    sdfCircleUnit.mvp = vpMat;
    sdfCircleUnit.draw();

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
    sdfMultiCirclesUnit.mvp = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfMultiCirclesUnit.objMat.setTo(360, 130, 200, 200);
    sdfMultiCirclesUnit.color = {0.0, 0.7, 0.7f, 1.0f};
    sdfMultiCirclesUnit.mvp = vpMat;
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