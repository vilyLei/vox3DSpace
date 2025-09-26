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

    redFormatexDrawUnit.color = {0.3f, 0.0, 0.3f, 1.0f};
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

void  OglTestScene::initVoassScene(){

    
    using namespace Voass::Render;

    //Gpu::DrawingUnit sdfCircleUnit{};
    bool colorClip      = true;
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


    sdfRoundedRectUnit.color = {0.5f, 0.2, 0.3f, 1.f};
    sdfRoundedRectUnit.objMat.setTo(300, 260, 200, 200);
    Gpu::buildSDFDrawUnit(sdfRoundedRectUnit, Shader::SDFShapeType::RoundedRect, colorClip);
    return;

    sdfTriangleUnit.color = {0.4f, 0.2, 0.2f, 1.f};
    sdfTriangleUnit.objMat.setTo(300, 300, 200, 200);
    Gpu::buildSDFDrawUnit(sdfTriangleUnit, Shader::SDFShapeType::Triangle);
    

}
void OglTestScene::renderVoass(const Voxol::Math::Mat33& vpMat) {

    
    sdfCircleUnit.mvp = vpMat;
    sdfCircleUnit.draw();

    sdfCircleUnit.color = {0.7f, 0.7, 0.7f, 1.0f};
    sdfCircleUnit.mvp = vpMat;
    sdfCircleUnit.draw();

    sdfMultiCirclesUnit.mvp = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfMultiCirclesUnit.color = {0.7f, 0.7, 0.7f, 1.0f};
    sdfMultiCirclesUnit.mvp = vpMat;
    sdfMultiCirclesUnit.draw();

    sdfSectorUnit.mvp = vpMat;
    sdfSectorUnit.draw();

    sdfRingUnit.mvp = vpMat;
    sdfRingUnit.draw();

    sdfRoundedRectUnit.mvp = vpMat;
    sdfRoundedRectUnit.draw();

    sdfTriangleUnit.mvp = vpMat;
    sdfTriangleUnit.draw();
    //
}
} // namespace Voxol::Test