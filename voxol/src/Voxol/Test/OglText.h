#ifndef VOXOL_OGL_TEXT_H
#define VOXOL_OGL_TEXT_H

#include "OglResUtils.h"
#include "OglImage.h"
#include <vector>
#include <ft2build.h>
#include <string>

#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

namespace Voxol::Test
{
struct OglTextFT
{

public:
    FT_Library ft   = nullptr;
    FT_Face    face = nullptr;
    ~OglTextFT();
    void dispose();
};

class OglTextGlyphBuilder
{
public:
    OglTextGlyphBuilder() = default;
    virtual ~OglTextGlyphBuilder() = default;

public:
    bool initFont(const std::string& fontPath = "C:/Windows/Fonts/SimHei.ttf");
    RawData::TextGlyphData createGlyph(char32_t ch32, int pixelSize, bool useSubpixel = false);
    RawData::TextGlyphData createGlyph(char ch32, int pixelSize, bool useSubpixel = false);

    RawData::TextGlyphData testBuildGlyph();

private:
    RawData::TextGlyphData createGlyphData(bool useSubpixel);
    OglTextFT              mFT{};
};

/// single line or multiple lines text field class;
class OglTextField
{
public:
    OglTextField()          = default;
    virtual ~OglTextField() = default;

public:
    void testInit(OglTextGlyphBuilder& builder);
    void render(const Voxol::Math::Mat33& projM);

private:
    std::vector<Gpu::DrawingUnit> mUnits{};
    std::vector<RawData::TextGlyphData> mGlyphs{};
};
class MSDFText
{
public:
    MSDFText()              = default;
    virtual ~MSDFText() = default;

public:
    void initialize(const std::string& atlasImgPath, const std::string& jsonPath);
    std::unordered_map<int, RawData::MSDFGlyph> loadGlyphs(const std::string& jsonFile);

private:
    RawData::Image2DBytesData mAtlasImgData{};
    RawData::MSDFAtlas        mMSDFAtlas{};

};
} // namespace Voxol::Test
#endif