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

const char* sdfFragSourceFuncs = R"(
    
float sdfCircle(float radius, vec2 center, vec2 xy) {
    return length(xy - center) - radius;
}
// ---------------- Ring / Donut ----------------
float sdfRing(vec2 p, float radius, float thickness) {
    return abs(length(p) - radius) - thickness * 0.5;
}

// ---------------- Rectangle ----------------
float sdfRect(vec2 p, vec2 size) {
    vec2 d = abs(p) - size;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// ---------------- Rounded Rectangle ----------------
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

// ---------------- Star ----------------
float sdfStar(vec2 p, float rOuter, float rInner, int n) {
    float angle = atan(p.y, p.x);
    float radius = length(p);
    float k = float(n)*0.5;
    float m = cos(mod(angle*k,3.14159265) - 3.14159265*0.5);
    float d = radius - mix(rOuter, rInner, m);
    return d;
}

// ---------------- Sector ----------------
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
)";


const char* sdfCircleFragSource = R"(
  
void main()
{
    vec2 center = vec2(0.5, 0.5);
    float d = sdfCircle(0.5, center, v_uv);
    
    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";

const char* sdfMultiCirclesFragSource = R"(
  
void main()
{
    vec2 center0 = vec2(0.25, 0.25);
    float d0 = sdfCircle(0.2, center0, v_uv);
    vec2 center1 = vec2(0.65, 0.45);
    float d1 = sdfCircle(0.3, center1, v_uv);
    float d = smoothUnion(d0, d1, 0.2);
    float alpha = aa(d) * u_color.a;

    fragColor = vec4(u_color.rgb * alpha, alpha);
}
)";

const char* getSdfVertShdCode() {
    return sdfVertSource;
}
std::string source{};

const char* getSdfFragShdCode(SDFShapeType type)
{
    std::string head = sdfFragSourceHead;
    std::string funcs = sdfFragSourceFuncs;
    source = head + funcs;
    switch (type)
    {
        case Voass::Render::Shader::SDFShapeType::MultiCircles:
            source += sdfMultiCirclesFragSource;
            break;
        case Voass::Render::Shader::SDFShapeType::Ring:
            break;
        case Voass::Render::Shader::SDFShapeType::Rect:
            break;
        case Voass::Render::Shader::SDFShapeType::RoundedRect:
            break;
        case Voass::Render::Shader::SDFShapeType::Triangle:
            break;
        case Voass::Render::Shader::SDFShapeType::Star:
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