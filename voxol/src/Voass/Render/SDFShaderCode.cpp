#include "SDFShaderCode.h"
#include <cstdio>
namespace Voass::Render {
namespace Shader {

	
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


const char* sdfFragSourceHead = R"(#version 330 core
    precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
out vec4          fragColor;

float aa(float d) {
    float factor = fwidth(d);
    return clamp(0.5 - d / factor, 0.0, 1.0);
}
)";


const char* sdfFragSourceClipDef = R"(

#define SDF_COLOR_CLIP 1

vec4 clipSdfColor(vec4 c4, vec4 bgColor4, float d) {
    return d > 0.3 ? vec4(mix(bgColor4.xyz, c4.xyz, d), c4.w) : vec4(bgColor4.xyz, 0.0);
}
)";

const char* sdfFragSourceFuncs = R"(
    
vec4 buildFragColor(vec4 color4, float d) {
    d = aa(d);
#ifndef SDF_COLOR_CLIP
    float alpha = d * color4.a;
    return vec4(color4.rgb * alpha, alpha);
#else
    vec4 fgColor = vec4(0.95,0.95,0.95, 1.0);
    return clipSdfColor(color4, fgColor, d);
#endif
}

float sdfCircleBase(float radius, vec2 center, vec2 xy) {
    return length(xy - center) - radius;
}
float sdfCircle(vec2 p, float r) {
    return length(p) - r;
}

float sdfRing(vec2 p, float radius, float thickness) {
    return abs(length(p) - radius) - thickness * 0.5;
}

float sdfRect(vec2 p, vec2 size) {
    vec2 d = abs(p) - size;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdfRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size;
    return length(max(d, 0.0)) - radius;
}

// ---------------- Triangle ----------------
// Equilateral triangle centered at origin
float sdfTriangle(vec2 p, float size) {
    const float k = sqrt(3.0);
    p.x = abs(p.x) - size;
    p.y = p.y + size/k;
    if( p.x + k*p.y > 0.0 ) p = vec2(p.x - k*p.y, -k*p.x - p.y)/2.0;
    p.x -= clamp(p.x, -2.0*size, 0.0);
    return -length(p)*sign(p.y);
}


float sdfStar(vec2 p, float rOuter, float rInner, int n) {
    float angle = atan(p.y, p.x);
    float radius = length(p);
    float k = float(n)*0.5;
    float m = cos(mod(angle*k,3.14159265) - 3.14159265*0.5);
    float d = radius - mix(rOuter, rInner, m);
    return d;
}

float sdfSector(vec2 p, float radius, vec2 dirStart, vec2 dirEnd) {
    float len = length(p);
    float dRadius = len - radius;
    float sideStart = - (dirStart.x * p.y - dirStart.y * p.x); // cross(dirStart, p)
    float sideEnd   =   (dirEnd.x   * p.y - dirEnd.y   * p.x); // cross(dirEnd, p)
    float dAngle = max(sideStart, sideEnd);
    return max(dRadius, dAngle);
}

float intersect(float a, float b) { return max(a, b); }
float union(float a, float b) { return min(a, b); }
float subtract(float a, float b) { return max(a, -b); }

float smoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) - k*h*(1.0 - h);
}
float smoothSubtract(float d1, float d2, float k) {
    return smoothUnion(d1, -d2, k);
}
float smoothIntersect(float d1, float d2, float k) {
    return -smoothUnion(-d1, -d2, k);
}
float union4(float d0, float d1, float d2, float d3) {
    return min(min(d0, d1), min(d2, d3));
}
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}
float smoothUnion4(float d0, float d1, float d2, float d3, float k) {
    float d = smin(d0, d1, k);
    d = smin(d, d2, k);
    d = smin(d, d3, k);
    return d;
}

vec4 smoothUnion(vec4 d1, vec4 d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2.w - d1.w) / k, 0.0, 1.0);
    float d = mix(d2.w, d1.w, h) - k * h * (1.0 - h);
    vec3 col = mix(d1.xyz, d2.xyz, h);
    return vec4(col, d);
}

vec4 smoothUnionVec4(vec4 c1, vec4 c2, float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(c1, c2, h);
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
    
    vec4 c0 = vec4(u_color.xyz, d0);
    vec4 c1 = vec4(vec3(0.0,0.5, 0.0), d1);
    float d = smoothUnion(d0, d1, 0.02);
    //vec4 cd = smoothUnion(c0, c1, 0.2);
    //float d = cd.w;
    //cd.a = u_color.a;
    vec4 cd = smoothUnionVec4(vec4(0.0, 0.5, 0.0, 0.5), u_color, d0, d1, 0.2);
    fragColor = buildFragColor(cd, d);
}
)";


const char* sdfRingFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfRing(v_uv - center, 0.4, 0.1);

    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";


const char* sdfSectorFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfSector(v_uv - center, 0.5, vec2(-1.0, 0.2), vec2(0.5, 0.5));

    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";


const char* sdfRoundedRectFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = buildRoundRect(v_uv, center, vec2(0.3, 0.3), vec4(0.2, 0.1, 0.4, 0.1));

    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
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

    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";

const char* getSdfVertShdCode() {
    return sdfVertSource;
}
std::string source{};

const char* getSdfFragShdCode(SDFShapeType type, bool clip)
{
    std::string head = sdfFragSourceHead;
    std::string funcs = sdfFragSourceFuncs;
    source = head;
    if (clip)
    {
        source += sdfFragSourceClipDef;
    }
    source += funcs;

    switch (type)
    {
        case Voass::Render::Shader::SDFShapeType::MultiCircles:
            source += sdfMultiCirclesFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Ring:
            source += sdfRingFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Rect:
            source += sdfRingFragSource;
            break;
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