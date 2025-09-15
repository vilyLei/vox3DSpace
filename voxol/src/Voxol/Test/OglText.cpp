#include "OglText.h"

#include <string>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Voxol::Test
{

int OglText::initialize()
{

    std::string fontPath = "C:/Windows/Fonts/arial.ttf";
    fontPath = "C:/Windows/Fonts/SimHei.ttf";

    // Init FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "FT_Init_FreeType failed\n";
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.data(), 0, &face))
    {
        std::cerr << "FT_New_Face failed for " << fontPath << "\n";
        FT_Done_FreeType(ft);
        return -1;
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

    char32_t ch          = U'жа';
    FT_UInt  glyph_index = FT_Get_Char_Index(face, ch);
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER))
    {
        std::cerr << "FT_Load_Char failed\n";
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return -1;
    }

    FT_GlyphSlot g = face->glyph;

    // Copy into continuous buffer respecting pitch
    glyphWidth   = g->bitmap.width;
    glyphHeight  = g->bitmap.rows;
    auto& bitmap = glyphBuffer;
    if (glyphWidth > 0 && glyphHeight > 0)
    {
        bitmap.resize(glyphWidth * glyphHeight);
        for (int row = 0; row < glyphHeight; ++row)
        {
            unsigned char* src = g->bitmap.buffer + row * g->bitmap.pitch;
            unsigned char* dst = bitmap.data() + row * glyphWidth;
            // copy bmpW bytes from src (pitch may be larger)
            memcpy(dst, src, glyphWidth);
        }
    }
    else
    {
        std::cerr << "Empty glyph bitmap\n";
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 1;
}
} // namespace Voxol::Test