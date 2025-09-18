#ifndef VOXOL_OGL_TEXT_H
#define VOXOL_OGL_TEXT_H

#include "OglResUtils.h"
#include <vector>
#include <ft2build.h>
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

class OglText
{
public:
    OglText() = default;
    virtual ~OglText() = default;

public:
    bool initFont(const std::string& fontPath = "C:/Windows/Fonts/SimHei.ttf");
    RawData::TextGlyphData createGlyph(char32_t ch32, int pixelSize, bool useSubpixel = false);
    RawData::TextGlyphData createGlyph(char ch32, int pixelSize, bool useSubpixel = false);

    RawData::TextGlyphData testBuildGlyph();

private:
    RawData::TextGlyphData createGlyphData(bool useSubpixel);
    OglTextFT              mFT{};
};

} // namespace Voxol::Test
#endif