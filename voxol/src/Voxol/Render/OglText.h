#ifndef VOXOL_RENDER_OGL_TEXT_H
#define VOXOL_RENDER_OGL_TEXT_H

#include "OglGpuResUtils.h"
#include "OglImage.h"
#include "../Math/VxRect.h"
#include <vector>
#include <ft2build.h>
#include <string>
#include <array>

#include FT_FREETYPE_H
#include FT_LCD_FILTER_H

namespace Voxol::Render
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
    OglTextGlyphBuilder()          = default;
    virtual ~OglTextGlyphBuilder() = default;

public:
    bool                   initFont(const std::string& fontPath = "C:/Windows/Fonts/SimHei.ttf");
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
    std::vector<Gpu::DrawingUnit>       mUnits{};
    std::vector<RawData::TextGlyphData> mGlyphs{};
};

class MSDFText
{
public:
    using SP = std::shared_ptr<MSDFText>;
    using WP = std::weak_ptr<MSDFText>;
    using UP = std::unique_ptr<MSDFText>;

public:
    static MSDFText::SP make();

public:
    MSDFText()  = default;
    ~MSDFText() = default;

public:
    void                                        initialize(const std::string& atlasImgPath, const std::string& jsonPath);
    std::unordered_map<int, RawData::MSDFGlyph> loadGlyphs(const std::string& jsonFile);
    void                                        buildText(const std::string&             text,
                                                          std::vector<Gpu::DrawingUnit>& units,
                                                          const Voxol::Math::Vec2&       pos      = {},
                                                          float                          fontSize = 20,
                                                          const std::array<float, 4>&    color    = {0, 0, 0, 1});
    void                                        createDrawUnitsFromText(const std::string&             text,
                                                                     std::vector<Gpu::DrawingUnit>& units,
                                                                     const Voxol::Math::Vec2&       pos      = {},
                                                                     float                          fontSize = 20);
    void                                        buildDrawingRes();
    std::vector<Math::Bounds>                   getGlyphBoundsWithText(const std::string& text, float fontSize, const Math::Vec2& pos);
    Math::Bounds                                calcStringBounds(const std::string& text, float fontSize, const Math::Vec2& pos);
    Math::Bounds                                getGlyphBounds(int32_t glyphChar, float fontSize, const Math::Vec2& pos);
    void                                        buildDrawingUnitWithGlyph(int32_t glyphChar, Gpu::DrawingUnit& unit);
    Gpu::DrawingUnit&                           getDrawingUnitWithGlyphAt(int32_t glyphChar);
    void                                        destory();

    inline const RawData::Image2DBytesData& getAtlasImage() const
    {
        return mAtlasImgData;
    }
    inline const RawData::MSDFAtlas& getAtlas() const
    {
        return mMSDFAtlas;
    }

private:
    RawData::Image2DBytesData     mAtlasImgData{};
    RawData::MSDFAtlas            mMSDFAtlas{};
    Gpu::DrawingUnit              mDrawingUnitGlyphA{};
    std::vector<Gpu::DrawingUnit> mDrawingUnitGlyphs{};
    std::vector<Math::Bounds>     bvs;
};

} // namespace Voxol::Render
#endif