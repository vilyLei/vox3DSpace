#include "SDFShaderCode.h"
#include <cstdio>
#include <fstream>
#include <filesystem>

namespace Voass::Render {
namespace Shader {

std::string loadShaderCodeFromFile(const std::string& fileName)
{
    auto filePath = std::filesystem::path(SRC_DIR) / "assets/shaderCode/";
    auto path     = filePath.string() + fileName;
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(size, '\0');
    file.read(content.data(), size);
    return content;
}
	
const char* sdfVertSource = R"(#version 330 core
precision highp float;

layout(location = 0) in vec4 a_pos;

uniform mat3 u_matrix;
out vec2 v_uv;
void main() {
    v_uv = a_pos.zw;
    mat3 trans = u_matrix;
    vec3 pos = trans * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
)";
const char* sdfFragSource = R"(#version 330 core
    precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
out vec4          fragColor;

float circleSdf(float radius, vec2 center, vec2 xy) {
    return length(xy - center) - radius;
}

float aa(float d) {
    float factor = fwidth(d);
    return clamp(0.5 - d / factor, 0.0, 1.0);
}

void              main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = circleSdf(0.5, center, v_uv);
    
    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";

const char* sdfFragSourceClipDef = R"(

#define SDF_COLOR_CLIP 1

vec4 clipSdfColor(vec4 c4, vec4 bgColor4, float d) {
    return d >= 1.0 ? vec4(bgColor4.xyz, c4.w * d) : vec4(bgColor4.xyz, 0.0);
}
)";

const char* sdfFragColorBuild = R"(

vec4 buildFragColor(vec4 color4, float d) {
    d = aa(d);
#ifndef SDF_COLOR_CLIP
    float alpha = d * color4.a;
    return vec4(color4.rgb * alpha, alpha);
#else
    return clipSdfColor(color4, color4, d);
#endif
}

)";

const char* sdfFragSourceRoundedRetFuncs = R"(
    float sdfRectSubInnerCircleCorner(vec2 gp, vec2 gcornerPos, vec2 rectHalfSize, vec2 direc, float radius) {
    
    radius = max(radius, 0.0);
    radius = min(min(rectHalfSize.x * 2.0f, rectHalfSize.y * 2.0f), radius);
    vec2 rhalfSize = vec2(radius * 0.5, radius * 0.5);
    vec2 grectcv = gcornerPos + rhalfSize * direc;
    float d0 = sdfRect(gp - grectcv, rhalfSize);
    vec2 gcirclecv = gcornerPos + vec2(radius, radius) * direc;
    float d1 = sdfCircle(gp - gcirclecv, radius);

    float d = subtract(d0, d1);
    return d;
}

float buildRoundRect(vec2 globalPV, vec2 rectCV, vec2 rectHalfSize, vec4 radius4) {
    // lt
    vec2 ltGPos = rectCV - rectHalfSize;
    vec2 rHaflSize = rectHalfSize * 0.5;
    float d0 = sdfRectSubInnerCircleCorner(globalPV, ltGPos, rHaflSize, vec2(1.0,1.0), radius4.x);
    // rt
    ltGPos = vec2(rectCV.x + rectHalfSize.x, rectCV.y - rectHalfSize.y);    
    float d1 = sdfRectSubInnerCircleCorner(globalPV, ltGPos, rHaflSize, vec2(-1.0,1.0), radius4.y);
    // rb
    ltGPos = rectCV + rectHalfSize;    
    float d2 = sdfRectSubInnerCircleCorner(globalPV, ltGPos, rHaflSize, vec2(-1.0,-1.0), radius4.z);
    // lb
    ltGPos = vec2(rectCV.x - rectHalfSize.x, rectCV.y + rectHalfSize.y);    
    float d3 = sdfRectSubInnerCircleCorner(globalPV, ltGPos, rHaflSize, vec2(1.0,-1.0), radius4.w);    
    float d = union4(d0, d1, d2, d3);

    float rd = sdfRect(globalPV - rectCV, rectHalfSize);
    //d =  subtract(rd, d);
    d = max(rd, -d);
    return d;
}
)";

const char* sdfCircleFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfCircle( v_uv - center, 0.5 );
    
    fragColor = buildFragColor(u_color, d);
}
)";

const char* sdfMultiCirclesFragSource = R"(
  
void main()
{
    vec2 center0 = vec2(0.25, 0.25);
    float d0 = sdfCircle(v_uv - center0, 0.2);
    vec2 center1 = vec2(0.65, 0.45);
    float d1 = sdfCircle(v_uv - center1, 0.3);
    float d = smoothUnion(d0, d1, 0.02);
    
    //vec4 c0 = vec4(u_color.xyz, d0);
    //vec4 c1 = vec4(vec3(0.0,0.5, 0.0), d1);
    //vec4 cd = smoothUnionVec4(vec4(0.0, 0.5, 0.0, 0.5), u_color, d0, d1, 0.2);
    //fragColor = buildFragColor(cd, d);
    fragColor = buildFragColor(u_color, d);
    fragColor = dithering(fragColor, v_uv);
}
)";


const char* sdfRingFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfRing(v_uv - center, 0.4, 0.1);

    fragColor = buildFragColor(u_color, d) * vec4(v_uv.xy, 1.0, 1.0);
}
)";


const char* sdfSectorFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfSector(v_uv - center, 0.5, vec2(-1.0, 0.2), vec2(0.5, 0.5));

    fragColor = buildFragColor(u_color, d);
}
)";


const char* sdfRoundedRectFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    //float d = buildRoundRect(v_uv, center, vec2(0.5, 0.5), vec4(0.2, 0.1, 0.4, 0.1));
    float d = sdfRoundRect(v_uv - center, vec2(0.5, 0.5), vec4(0.2, 0.1, 0.4, 0.1));
    fragColor = buildFragColor(u_color, d);
}
)";


const char* sdfTriangleFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    vec2 center1 = vec2(0.7, 0.6);
    float d = sdfTriangle(v_uv - center, 0.3);

    float d1 = sdfCircle(v_uv - center1, 0.2);
    //d = smoothIntersect(-d1, -d, 0.2);
    d = smoothUnion(d, d1, 0.2);

    //float alpha = aa(d) * u_color.a;
    //fragColor = vec4(u_color.rgb * alpha, alpha);

    fragColor = buildFragColor(u_color, d);
}
)";

const char* sdfRectFragSource = R"(#version 330 core
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    outColor = u_color;
})";
const char* getSdfVertShdCode() {
    return sdfVertSource;
}
std::string source{};

const char* getSdfFragShdCode(SDFShapeType type, bool clip)
{
    static std::string sdfDefaultShapeStr;
    static std::string sdfFragSourceHeadStr;
    static std::string sdfFragSourceFuncsStr;
    if (type == Voass::Render::Shader::SDFShapeType::Rect)
    {
        return sdfRectFragSource;
    }
#ifdef NATIVE_RUNTIME
    if (type == Voass::Render::Shader::SDFShapeType::DefaultShape)
    {
        if (sdfDefaultShapeStr.empty())
        {
            sdfDefaultShapeStr = loadShaderCodeFromFile("sdfDefaultShape.glsl");
        }
        return sdfDefaultShapeStr.c_str();
    }

    if (sdfFragSourceHeadStr.empty())
    {
        sdfFragSourceHeadStr = loadShaderCodeFromFile("sdfFragSourceHead.glsl");
    }
    if (sdfFragSourceFuncsStr.empty())
    {
        sdfFragSourceFuncsStr = loadShaderCodeFromFile("sdfFragSourceFuncs.glsl");
    }
#endif
    source = sdfFragSourceHeadStr;
    if (clip)
    {
        source += sdfFragSourceClipDef;
    }
    source += sdfFragSourceFuncsStr;
    source += sdfFragColorBuild;

    switch (type)
    {
        case Voass::Render::Shader::SDFShapeType::MultiCircles:
            source += sdfMultiCirclesFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Ring:
            source += sdfRingFragSource;
            break;
        //case Voass::Render::Shader::SDFShapeType::Rect:
        //    source += sdfRingFragSource;
        //    break;
        case Voass::Render::Shader::SDFShapeType::RoundedRect:
            source += sdfFragSourceRoundedRetFuncs;
            source += sdfRoundedRectFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Triangle:
            source += sdfTriangleFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Star:
            source += sdfRingFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Sector:
            source += sdfSectorFragSource;
            break;
        default:
            source += sdfCircleFragSource;
            break;
    }
    //printf("source: \n%s\n", source.data());
    return source.data();
}
}
}