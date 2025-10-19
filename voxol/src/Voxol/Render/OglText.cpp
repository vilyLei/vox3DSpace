#include "OglText.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

namespace Voxol::Render
{

OglTextFT ::~OglTextFT()
{
    dispose();
}
void OglTextFT::dispose()
{
    if (face == nullptr)
        FT_Done_Face(face);
    if (ft == nullptr)
        FT_Done_FreeType(ft);

    face = nullptr;
    ft   = nullptr;
}


bool OglTextGlyphBuilder::initFont(const std::string& fontPath)
{
    if (mFT.face != nullptr)
        return true;

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("FT_Init_FreeType failed\n");
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.data(), 0, &face))
    {
        printf("FT_New_Face failed for %s\n", fontPath.c_str());
        FT_Done_FreeType(ft);
        return false;
    }
    mFT.face = face;
    mFT.ft   = ft;
    return true;
}

RawData::TextGlyphData OglTextGlyphBuilder::createGlyph(char32_t ch32, int pixelSize, bool useSubpixel)
{
    FT_Set_Pixel_Sizes(mFT.face, 0, pixelSize);
    FT_UInt glyph_index = FT_Get_Char_Index(mFT.face, ch32);

    int loadFlags = FT_LOAD_RENDER;
    if (useSubpixel)
    {
        loadFlags |= FT_LOAD_TARGET_LCD;
        FT_Library_SetLcdFilter(mFT.face->glyph->library, FT_LCD_FILTER_DEFAULT);
    }

    if (FT_Load_Glyph(mFT.face, glyph_index, loadFlags))
    {
        printf("FT_Load_Glyph failed\n");
        return {};
    }
    return createGlyphData(useSubpixel);
}
RawData::TextGlyphData OglTextGlyphBuilder::createGlyph(char ch, int pixelSize, bool useSubpixel)
{
    FT_Set_Pixel_Sizes(mFT.face, 0, pixelSize);
    FT_UInt glyph_index = FT_Get_Char_Index(mFT.face, ch);

    int loadFlags = FT_LOAD_RENDER;
    if (useSubpixel)
    {
        loadFlags |= FT_LOAD_TARGET_LCD;
        FT_Library_SetLcdFilter(mFT.face->glyph->library, FT_LCD_FILTER_DEFAULT);
    }
    if (FT_Load_Char(mFT.face, ch, loadFlags))
    {
        printf("FT_Load_Char failed\n");
        return {};
    }
    return createGlyphData(useSubpixel);
}

RawData::TextGlyphData OglTextGlyphBuilder::createGlyphData(bool useSubpixel)
{

    FT_GlyphSlot g   = mFT.face->glyph;
    auto&        bmp = g->bitmap;

    int bmpW = bmp.width;
    int bmpH = bmp.rows;

    RawData::TextGlyphData glyphData{};
    auto&                  imgData = glyphData.image;
    imgData.width                  = bmpW;
    imgData.height                 = bmpH;
    auto& bitmap                   = imgData.buffer;

    if (bmpW > 0 && bmpH > 0)
    {
        glyphData.bearingX = g->bitmap_left;
        glyphData.bearingY = g->bitmap_top;
        glyphData.advance  = static_cast<unsigned int>(g->advance.x >> 6);

        bitmap.resize(bmpW * bmpH);

        int pitch    = g->bitmap.pitch;
        int absPitch = std::abs(pitch);

        for (int row = 0; row < bmpH; ++row)
        {
            unsigned char* src;
            if (pitch > 0)
                src = g->bitmap.buffer + row * pitch;
            else
                src = g->bitmap.buffer + (bmpH - 1 - row) * absPitch;

            unsigned char* dst = bitmap.data() + row * bmpW;
            memcpy(dst, src, bmpW);
        }

        if (useSubpixel)
        {
            imgData.format = GL_RGB;
            imgData.width  = bmpW / 3;
        }
        else
        {
            imgData.format = GL_RED;
        }
    }
    else
    {
        printf("Empty glyph bitmap\n");
    }

    return glyphData;
}

RawData::TextGlyphData OglTextGlyphBuilder::testBuildGlyph()
{

    std::string fontPath = "C:/Windows/Fonts/arial.ttf";
    fontPath             = "C:/Windows/Fonts/SimHei.ttf";

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("FT_Init_FreeType failed\n");
        return {};
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.data(), 0, &face))
    {
        printf("FT_New_Face failed for %s\n", fontPath.c_str());
        FT_Done_FreeType(ft);
        return {};
    }

    // set pixel size
    const int pixelSize = 64;
    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    // choose the glyph to render
    //const char ch = 'A';
    //if (FT_Load_Char(face, ch, FT_LOAD_RENDER))
    //{
    //    std::cerr << "FT_Load_Char failed\n";
    //    FT_Done_Face(face);
    //    FT_Done_FreeType(ft);
    //    return -1;
    //}

    char32_t ch          = U'中';
    FT_UInt  glyph_index = FT_Get_Char_Index(face, ch);
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER))
    {
        printf("FT_Load_Char failed\n");
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return {};
    }

    RawData::TextGlyphData glyphData{};
    auto&                  glyphImg = glyphData.image;

    FT_GlyphSlot g = face->glyph;

    // Copy into continuous buffer respecting pitch
    auto glyphWidth  = g->bitmap.width;
    auto glyphHeight = g->bitmap.rows;

    glyphImg.width  = glyphWidth;
    glyphImg.height = glyphHeight;

    auto& bitmap = glyphImg.buffer;
    if (glyphWidth > 0 && glyphHeight > 0)
    {
        bitmap.resize(glyphWidth * glyphHeight);
        for (int row = 0; row < glyphHeight; ++row)
        {
            unsigned char* src = g->bitmap.buffer + row * g->bitmap.pitch;
            unsigned char* dst = bitmap.data() + row * glyphWidth;
            memcpy(dst, src, glyphWidth);
        }
    }
    else
    {
        printf("Empty glyph bitmap\n");
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return glyphData;
}


void OglTextField::testInit(OglTextGlyphBuilder& builder)
{

    std::u32string chList = U"量中文ABCghPijkpqW";
    auto           total  = chList.size();
    mUnits.resize(total);
    mGlyphs.resize(total);

    float px    = 350;
    float py    = 380;
    float scale = 1;
    for (auto i = 0; i < total; i++)
    {
        auto ch    = chList[i];
        mGlyphs[i] = builder.createGlyph(ch, 32, false);
    }

    int lineAscent  = 0;
    int lineDescent = 0;

    for (auto i = 0; i < total; i++)
    {
        auto& chData = mGlyphs[i];
        lineAscent   = std::max(lineAscent, chData.bearingY);
        lineDescent  = std::max(lineDescent, chData.image.height - chData.bearingY);
    }
    for (auto i = 0; i < total; i++)
    {
        auto& chData = mGlyphs[i];
        auto& img    = chData.image;
        float xpos   = px + chData.bearingX * scale;
        //float  ypos       = py - (img.height - chData.bearingY) * scale;
        // float ypos = py - chData.bearingY * scale;
        auto  disY = (lineAscent - chData.bearingY) * scale;
        float ypos = py + disY;

        px += (chData.advance >> 6) * scale;

        float w = img.width * scale;
        float h = img.height * scale;

        auto& unit = mUnits[i];
        unit.color = {0.0f, 0.7f, 0.7f, 1.0f};
        unit.objMat.setTo(xpos, ypos, w, h);
        Gpu::buildGlyphTexDrawUnit(unit, chData);
    }
}
void OglTextField::render(const Voxol::Math::Mat33& projM)
{

    for (auto& unit : mUnits)
    {
        unit.mvp = projM;
        unit.draw();
    }
}
/// thanks: https://github.com/Chlumsky/msdf-atlas-gen/releases
/// msdf-atlas-gen.exe -font "C:\Windows\Fonts\arial.ttf" -imageout "atlas.png" -json "atlas.json" -type msdf
///

void MSDFText::initialize(const std::string& atlasImgPath, const std::string& jsonPath)
{
    OglImage imgObj{};
    //imgObj.loadPNGFromAssets("msdf/arial_atlas.png");

    mAtlasImgData = imgObj.loadPNGFromAssets(atlasImgPath);

    mAtlasImgData.tex = ResUtils::createTextureFromImageBytes(mAtlasImgData.width, mAtlasImgData.height, mAtlasImgData.buffer);

    auto texPath = std::filesystem::path(SRC_DIR) / "assets/";
    loadGlyphs(texPath.string() + jsonPath);
}
std::unordered_map<int, RawData::MSDFGlyph> MSDFText::loadGlyphs(const std::string& jsonFile)
{
    std::ifstream  f(jsonFile);
    nlohmann::json j;
    f >> j;

    mMSDFAtlas.reset();

    auto& atlas         = mMSDFAtlas;
    atlas.width         = j["atlas"]["width"];
    atlas.height        = j["atlas"]["height"];
    atlas.distanceRange = j["atlas"]["distanceRange"];
    atlas.emSize        = j["metrics"]["emSize"];
    atlas.lineHeight    = j["metrics"]["lineHeight"];
    atlas.ascender      = j["metrics"]["ascender"];
    atlas.descender     = j["metrics"]["descender"];

    auto& glyphMap = atlas.glyphs;
    for (auto& g : j["glyphs"])
    {
        RawData::MSDFGlyph glyph;
        glyph.advance = g["advance"];
        if (g.contains("planeBounds"))
        {
            glyph.planeLeft   = g["planeBounds"]["left"];
            glyph.planeBottom = g["planeBounds"]["bottom"];
            glyph.planeRight  = g["planeBounds"]["right"];
            glyph.planeTop    = g["planeBounds"]["top"];
        }
        else
        {
            glyph.planeLeft = glyph.planeBottom = glyph.planeRight = glyph.planeTop = 0;
        }
        if (g.contains("atlasBounds"))
        {
            auto& ab          = g["atlasBounds"];
            glyph.atlasLeft   = float(ab["left"]) / atlas.width;
            glyph.atlasBottom = float(ab["bottom"]) / atlas.height;
            glyph.atlasRight  = float(ab["right"]) / atlas.width;
            glyph.atlasTop    = float(ab["top"]) / atlas.height;
        }
        else
        {
            glyph.atlasLeft = glyph.atlasBottom = glyph.atlasRight = glyph.atlasTop = 0;
        }
        glyphMap[g["unicode"]] = glyph;
    }
    return glyphMap;
}
void MSDFText::buildText(const std::string&             text,
                         std::vector<Gpu::DrawingUnit>& units,
                         const Voxol::Math::Vec2&       pos,
                         float                          fontSize,
                         const std::array<float, 4>&    color)
{
    if (text.empty()) return;

    units.clear();
    units.reserve(text.size());

    auto& atlas = mMSDFAtlas;
    float scale = fontSize / atlas.emSize;

    float penX = 0.0f;

    for (unsigned char c : text)
    {
        auto it = atlas.glyphs.find((int)c);
        if (it == atlas.glyphs.end()) continue;

        const RawData::MSDFGlyph& glyph = it->second;

        Gpu::DrawingUnit unit;

        float x0 = glyph.planeLeft * scale;
        float y1 = glyph.planeTop * scale;

        float pw = (glyph.planeRight - glyph.planeLeft) * scale;
        float ph = (glyph.planeTop - glyph.planeBottom) * scale;

        unit.color = color;
        // pos.y 就是基线
        unit.objMat.setTo(pos.x + penX + x0,
                          pos.y - y1,
                          pw, ph);

        Gpu::buildMSDFTexDrawUnit(unit, mAtlasImgData, glyph);

        units.push_back(unit);

        penX += glyph.advance * scale; // 横向推进
    }
}

void MSDFText::buildDrawingRes() {

    if (mDrawingUnitGlyphA.hasTexture())
    {
        return;
    }
    char a = 'A';
    auto& glyph = mMSDFAtlas.glyphs[(int)a];
    Gpu::buildMSDFTexDrawUnit(mDrawingUnitGlyphA, mAtlasImgData, glyph);
}


std::vector<Math::Bounds> MSDFText::getStringBounds(const std::string& text, float fontSize, const Math::Vec2& pos)
{
    auto& atlas = mMSDFAtlas;
    float scale = fontSize / atlas.emSize;

    float penX = 0.0f;
    bvs.resize(text.size());
    auto                      i = 0;
    for (unsigned char c : text)
    {
        auto it = atlas.glyphs.find((int)c);
        if (it == atlas.glyphs.end()) continue;

        const auto& glyph = it->second;

        float x0 = glyph.planeLeft * scale;
        float y1 = glyph.planeTop * scale;

        float pw = (glyph.planeRight - glyph.planeLeft) * scale;
        float ph = (glyph.planeTop - glyph.planeBottom) * scale;

        auto px = pos.x + penX + x0;
        auto py = pos.y - y1;
        // pos.y 就是基线
        bvs[i]  = {
                px,
                py,
                px + pw,
                py + ph
        };
        penX += glyph.advance * scale; // 横向推进
        i++;
    }
    return bvs;
}
Math::Bounds MSDFText::getGlyphBounds(int32_t glyphChar, float fontSize, const Math::Vec2& pos)
{

    auto& glyphs = mMSDFAtlas.glyphs;
    if (!glyphs.contains(glyphChar))
        return {};

    float scale = fontSize / mMSDFAtlas.emSize;
    auto& glyph = glyphs[glyphChar];

    float x0 = glyph.planeLeft * scale;
    float y1 = glyph.planeTop * scale;

    float pw = (glyph.planeRight - glyph.planeLeft) * scale;
    float ph = (glyph.planeTop - glyph.planeBottom) * scale;

    auto px = pos.x + x0;
    auto py = pos.y - y1;
    // pos.y 就是基线
    return {
        px,
        py,
        px + pw,
        py + ph};
}
void MSDFText::buildDrawingUnitWithGlyph(int32_t glyphChar, Gpu::DrawingUnit& unit)
{
    if (!mMSDFAtlas.glyphs.contains(glyphChar))
        return;

    auto& glyph = mMSDFAtlas.glyphs[glyphChar];
    unit.shader = mDrawingUnitGlyphA.shader;
    Gpu::buildMSDFTexDrawUnit(unit, glyph);
}

void MSDFText::destory()
{

}
} // namespace Voxol::Render