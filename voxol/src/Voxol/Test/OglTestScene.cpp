#include "OglTestScene.h"
#include "OglText.h"

namespace Voxol::Test
{


void OglTestScene::initScene()
{
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
    msdfText.buildText(text, msdfTextDrawUnits, {300, 100});
    
}
void OglTestScene::render(const Voxol::Math::Mat33& projM)
{
    baseDrawUnit.mvp = projM;
    baseDrawUnit.draw();

    texDrawUnit.mvp = projM;
    texDrawUnit.draw();

    redFormatexDrawUnit.mvp = projM;
    redFormatexDrawUnit.draw();

    glyphDrawUnit.mvp = projM;
    glyphDrawUnit.draw();

    pngUnit.mvp = projM;
    pngUnit.draw();
    
    for (auto& unit : msdfTextDrawUnits)
    {
        unit.mvp = projM;
        unit.draw();
    }
}

} // namespace Voxol::Test