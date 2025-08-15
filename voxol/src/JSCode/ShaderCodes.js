"use strict";

export const vertSourceScreenV3 = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;
uniform mat3 u_objMat;
void main() {
    vec3 pos = u_objMat * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;
export const vertSourceMVPV3 = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;
uniform mat3 u_objMat;
uniform mat3 u_viewMat;
uniform mat3 u_projMat;
void main() {
    mat3 trans = u_projMat * u_viewMat * u_objMat;
    vec3 pos = trans * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;

export const fragSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    outColor = u_color;
}
`;

export const vertTexMVPSource = `#version 300 es
precision highp float;

layout(location = 0) in vec4 a_pos;

uniform mat3 u_objMat;
uniform mat3 u_viewMat;
uniform mat3 u_projMat;

out vec2 v_uv;

void main() {
    v_uv = a_pos.zw;
    mat3 trans = u_projMat * u_viewMat * u_objMat;
    vec3 pos = trans * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;
export const fragTexSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
in vec2 v_uv;
uniform sampler2D u_tex0;
out vec4 fragColor;
void main() {
    fragColor = texture(u_tex0, v_uv) * u_color;
}
`;

export const fragPreMultAlphaTexSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
in vec2 v_uv;
uniform sampler2D u_tex0;
out vec4 fragColor;
void main() {
    fragColor = texture(u_tex0, v_uv);
    fragColor.rgb /= vec3(min(fragColor.a + 0.0001, 1.0));
    fragColor *= u_color;
}
`;

export function getVertSourceV3SegN(n) {

    const vertSourceV3SegN = `#version 300 es
precision highp float;

layout(location = 0) in vec3 a_pos;
uniform mat3 u_transforms[${n}];
out float v_idx;
void main() {
    v_idx = a_pos.z;
    int idx = int(a_pos.z);
    mat3 trans = u_transforms[idx];
    vec3 pos = trans * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;
    return vertSourceV3SegN;
}

export function getFragSourceSegN(n) {
    const fragSourceSegN = `#version 300 es
precision mediump float;
uniform vec4 u_colors[${n}];
in float v_idx;
out vec4 outColor;
void main() {
    outColor = u_colors[int(v_idx)];
}
`;
    return fragSourceSegN;
}