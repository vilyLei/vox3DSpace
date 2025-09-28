#ifndef VOXOL_OGL_RENDER_RES_UTILS_H
#define VOXOL_OGL_RENDER_RES_UTILS_H

#include "../Base/BaseDefine.h"
#include "../Math/Mat33.h"
#include "SDFShaderCode.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <unordered_map>

#include <GL/glew.h>
#include <glfw3.h>

namespace Voxol::Test
{
namespace ResUtils
{
//inline const char* vertShaderSource = "";
extern const char* vertShaderSource;
extern const char* fragShaderSource;
extern const char* vertTexSource;
extern const char* fragTexSource;

GLuint                      compileShader(GLenum type, const char* source);
GLuint                      createSahderProgram(const char* vertSource, const char* fragSource);
std::vector<float>          getVertsWithVEOSegN(int n);
std::vector<float>          getVertsWithUVVEOSegN(int n);

std::vector<float> getVertsWithUVVEO(float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1);
std::vector<float> getVertsWithUVVEOFlipY(float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1);
std::vector<unsigned short> getIndicesWithSegN(int n);
GLuint                      createTextureFromImageBytes(int imageWidth, int imageHeight, const std::vector<unsigned char>& buffer, GLint internalformat = GL_RGBA, GLint format = GL_RGBA, GLint alignment = 4);
std::vector<unsigned char>  createRGBAImgBytes(int imageWidth, int imageHeight);

} // namespace ResUtils
namespace RawData
{
struct Image2DBytesData
{
    int                        width          = 0;
    int                        height         = 0;
    GLint                      internalformat = GL_RGBA;
    GLint                      format         = GL_RGBA;
    std::vector<unsigned char> buffer{};
    GLuint                     tex = 0;
};
struct TextGlyphData
{
    int bearingX = 0;
    int bearingY = 0;
    unsigned int advance = 0;
    bool useSubpixel = false;
    Image2DBytesData image{};

};
struct MSDFGlyph
{
    float advance;
    float planeLeft, planeBottom, planeRight, planeTop;
    float atlasLeft, atlasBottom, atlasRight, atlasTop;

    //GLuint tex = 0;
};

struct MSDFAtlas
{
    int                  width, height;
    float                emSize, lineHeight, ascender, descender;
    float                distanceRange;
    std::unordered_map<int, MSDFGlyph> glyphs;
    void                               reset();
};

} // namespace RawData
namespace Gpu
{
struct ShdNode
{
    GLint               program   = GL_ZERO;
    GLint               matrixLoc = GL_ZERO;
    GLint               colorLoc  = GL_ZERO;
    std::vector<GLint>  texLocs{};
    std::vector<GLuint> textures{};
    void                bindGPU();
};

struct VertVSNode
{
    int componentSize   = 2;
    int componentStride = 2;

    std::vector<float> vs{};
};
struct VertNode
{
    GLuint                      vao = GL_ZERO;
    GLuint                      veo = GL_ZERO;
    std::vector<VertVSNode>     vsNodes{};
    std::vector<unsigned short> indices{};
    void                        buildBaseRes();
    void                        buildTexRes();
    void                        buildTexResUvs(float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1);
    void                        buildTexResFlipYUvs(float u0 = 0, float v0 = 0, float u1 = 1, float v1 = 1);
    void                        buildGPURes();
    void                        bindGPU();
    GLsizei                     indicesSize() const;
    void                        draw();
};

struct DrawingUnit
{
    VertNode vertex{};
    ShdNode  shader{};

    std::array<float, 4> color{1.0f, 1.0f, 1.0f, 1.0f};
    Voxol::Math::Mat33   objMat{};
    Voxol::Math::Mat33   mvp{};
    int                  blendMode = 1;
    bool                  colorClip = false;

    void bindGPU();
    GLuint getTextureAt(int index) const;
    void draw();
};

void buildBaseDrawUnit(DrawingUnit& unit);
void buildTexDrawUnit(DrawingUnit& unit, const RawData::Image2DBytesData& imgData = {});
void buildTexDrawUnitFromTex(DrawingUnit& unit, GLuint tex, bool uvFlipY = false);
void buildSDFDrawUnit(DrawingUnit& unit, Voass::Render::Shader::SDFShapeType type = Voass::Render::Shader::SDFShapeType::Circle, bool clip = false);
void buildMSDFTexDrawUnit(
    DrawingUnit&                     unit,
    const RawData::Image2DBytesData& imgData,
    const RawData::MSDFGlyph&        glyph);
void buildRedFormatTexDrawUnit(DrawingUnit& unit, const RawData::Image2DBytesData& imgData = {});
void buildGlyphTexDrawUnit(DrawingUnit& unit, const RawData::TextGlyphData& glyphData = {});

} // namespace Gpu
} // namespace Voxol::Test
#endif