#include "DrawingUnitStorage.h"
#include "OglText.h"

namespace Voxol::Render
{
DrawingUnitStorage::SP DrawingUnitStorage::make()
{
    auto sp = std::make_shared<DrawingUnitStorage>();
    return sp;
}

void DrawingUnitStorage::initalize(int total)
{
    if (!drawingUnits.empty())
        return;

    total = total < 128 ? 128 : total;
    drawingUnits.resize(total);

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

int32_t DrawingUnitStorage::getIdWithName(const std::string& name)
{
    if (!nameMap.contains(name))
        return -1;
    return nameMap[name];
}

Gpu::DrawingUnit& DrawingUnitStorage::getWithName(const std::string& name)
{
    auto id = nameMap[name];
    return drawingUnits[id];
}

int32_t DrawingUnitStorage::getIdWithType(DrawingUnitType type) const
{
    auto id = static_cast<int32_t>(type);
    return id;
}

bool DrawingUnitStorage::hasType(DrawingUnitType type) const
{
    return true;
}

Gpu::DrawingUnit& DrawingUnitStorage::getWithType(DrawingUnitType type)
{
    auto id = static_cast<int32_t>(type);
    return drawingUnits[id];
}
const Gpu::DrawingUnit& DrawingUnitStorage::getWithType(DrawingUnitType type) const
{
    auto id = static_cast<int32_t>(type);
    return drawingUnits[id];
}


bool DrawingUnitStorage::hasAt(int32_t id) const
{
    return id >= 0 && id < drawingUnits.size();
}
Gpu::DrawingUnit& DrawingUnitStorage::getAt(int32_t id)
{
    return drawingUnits[id];
}
Gpu::DrawingUnit& DrawingUnitStorage::operator[](int32_t id)
{
    return drawingUnits[id];
}
const Gpu::DrawingUnit& DrawingUnitStorage::getAt(int32_t id) const
{
    return drawingUnits[id];
}
const Gpu::DrawingUnit& DrawingUnitStorage::operator[](int32_t id) const
{
    return drawingUnits[id];
}

void DrawingUnitStorage::initVoassScene()
{

    using namespace Voass::Render;

    auto& rectUnit            = drawingUnits[0];
    auto& sdfCircleUnit       = drawingUnits[1];
    auto& sdfMultiCirclesUnit = drawingUnits[2];
    auto& sdfRingUnit         = drawingUnits[3];
    auto& sdfSectorUnit       = drawingUnits[4];
    auto& sdfRoundedRectUnit  = drawingUnits[5];
    auto& sdfTriangleUnit     = drawingUnits[6];
    auto& strokeShapeUnit     = drawingUnits[7];

    bool colorClip      = false;
    sdfCircleUnit.color = {0.9f, 0.0, 0.3f, 1.0f};
    sdfCircleUnit.objMat.setTo(100, 100, 200, 200);
    Render::Gpu::buildSDFDrawUnit(sdfCircleUnit, Shader::SDFShapeType::Circle, colorClip);

    sdfMultiCirclesUnit.color = {0.6f, 0.0, 0.3f, 1.0f};
    sdfMultiCirclesUnit.objMat.setTo(330, 100, 200, 200);
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


    rectUnit.color = {0.1f, 0.6, 0.3f, 1.0f};
    rectUnit.objMat.setTo(100, 100, 200, 80);
    Render::Gpu::buildSDFDrawUnit(rectUnit, Shader::SDFShapeType::Rect, colorClip);

    Render::Gpu::buildSDFDrawUnit(strokeShapeUnit, Shader::SDFShapeType::DefaultShape, colorClip);

    auto        id       = 8;
    std::string fileName = "ship01.glsl";
    auto&       unit_8   = drawingUnits[id];
    Render::Gpu::buildSDFDrawUnitWithName(unit_8, fileName, colorClip);
    nameMap[fileName] = id;

    drawingIDIndex = id;

}

} // namespace Voxol::Render