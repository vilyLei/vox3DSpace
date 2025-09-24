#include "SDFShaderCode.h"
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
void              main()
{
    fragColor = texColor * u_color;
    fragColor.xy *= v_uv.xy;
}
)";
const char* getSdfVertShdCode() {
    return sdfVertSource;
}
const char* getSdfFragShdCode(SDFShapeType type) {
    return sdfFragSource;
}
}
}