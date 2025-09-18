#include "OglText.h"

#include <string>
#include <iostream>

namespace Voxol::Test
{

OglTextFT ::~OglTextFT()
{
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


bool OglText::initFont(const std::string& fontPath)
{
    if (mFT.face != nullptr)
        return true;

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "FT_Init_FreeType failed\n";
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.data(), 0, &face))
    {
        std::cerr << "FT_New_Face failed for " << fontPath << "\n";
        FT_Done_FreeType(ft);
        return false;
    }
    mFT.face = face;
    mFT.ft   = ft;
    return true;
}

RawData::TextGlyphData OglText::createGlyph(char32_t ch32, int pixelSize, bool useSubpixel)
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
        std::cerr << "FT_Load_Glyph failed\n";
        return {};
    }
    return createGlyphData(useSubpixel);
}
RawData::TextGlyphData OglText::createGlyph(char ch, int pixelSize, bool useSubpixel)
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
        std::cerr << "FT_Load_Char failed\n";
        return {};
    }
    return createGlyphData(useSubpixel);
}

RawData::TextGlyphData OglText::createGlyphData(bool useSubpixel)
{

    FT_GlyphSlot g    = mFT.face->glyph;
    auto&        bmp  = g->bitmap;

    int          bmpW = bmp.width;
    int          bmpH = bmp.rows;

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
            imgData.width  = bmpW / 3; // 实际字符宽度
        }
        else
        {
            imgData.format = GL_RED;
        }
    }
    else
    {
        std::cerr << "Empty glyph bitmap\n";
    }

    return glyphData;
}

RawData::TextGlyphData OglText::testBuildGlyph()
{

    std::string fontPath = "C:/Windows/Fonts/arial.ttf";
    fontPath             = "C:/Windows/Fonts/SimHei.ttf";

    // Init FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "FT_Init_FreeType failed\n";
        return {};
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.data(), 0, &face))
    {
        std::cerr << "FT_New_Face failed for " << fontPath << "\n";
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
        std::cerr << "FT_Load_Char failed\n";
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
        std::cerr << "Empty glyph bitmap\n";
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return glyphData;
}
} // namespace Voxol::Test