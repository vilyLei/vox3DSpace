#ifndef VOXOL_OGL_TEXT_H
#define VOXOL_OGL_TEXT_H

#include "OglResUtils.h"
#include <vector>

namespace Voxol::Test
{
class OglText
{
public:
    OglText() = default;
    virtual ~OglText() = default;

public:
    RawData::TextGlyphData testBuildGlyph();

public:
    //int                        glyphWidth = 0;
    //int                        glyphHeight = 0;
    //std::vector<unsigned char> glyphBuffer{};
};

} // namespace Voxol::Test
#endif