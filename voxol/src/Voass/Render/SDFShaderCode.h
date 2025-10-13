#ifndef VOASS_RENDER_SDF_SDADER_CODE_H
#define VOASS_RENDER_SDF_SDADER_CODE_H
#include <string>
#include <cstdint>
#include <string>

namespace Voass::Render
{
namespace Shader
{

std::string loadShaderCodeFromFile(const std::string& fileName);

enum class SDFShapeType : uint8_t
{
    Circle       = 0x1,
    MultiCircles = 0x2,
    Ring         = 0x3,
    Rect         = 0x4,
    RoundedRect  = 0x5,
    Triangle     = 0x6,
    Star         = 0x7,
    Sector         = 0x8,
};

const char* getSdfVertShdCode();
const char* getSdfFragShdCode(SDFShapeType type, bool clip = false);
} // namespace Shader
} // namespace Voass::Render
#endif