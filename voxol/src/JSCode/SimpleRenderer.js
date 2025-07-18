function testDo() {
    console.log("testDo() ...\n");
}
export class SimpleRenderer {
    constructor(canvas) {
        this.gl = null;
    }
    initialize(glCtx) {

        this.gl = glCtx;

        testDo();

        console.log("SimpleRenderer::initialize() ...\n");

        const vertSource = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;

uniform mat3 u_matrix;

void main() {
    vec3 pos = u_matrix * vec3(a_pos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;

        const fragSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    //outColor = vec4(1.0, 0.8, 0.8, 1.0);
    outColor = u_color;
}
`;

        const vertTexSource = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv;

uniform mat3 u_matrix;
out vec2 v_uv;
void main() {
    v_uv = a_uv;
    vec3 pos = u_matrix * vec3(a_pos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;

        const fragTexSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
in vec2 v_uv;
uniform sampler2D u_tex;
out vec4 fragColor;
void main() {
    //outColor = vec4(1.0, 0.8, 0.8, 1.0);
    // outColor = u_color;
    fragColor = texture(u_tex, v_uv) * 0.5 + vec4(v_uv, 1.0, 1.0) * u_color * 0.5;
    // fragColor = texture(u_tex, vec2(0.5,0.5));
}
`;
    }

    createShader(gl, type, source) {
        const shader = gl.createShader(type);
        gl.shaderSource(shader, source);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(shader));
            gl.deleteShader(shader);
            return null;
        }
        return shader;
    }

    createProgram(gl, vs, fs) {
        const program = gl.createProgram();
        gl.attachShader(program, vs);
        gl.attachShader(program, fs);
        gl.linkProgram(program);
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            console.error(gl.getProgramInfoLog(program));
            return null;
        }
        return program;
    }

    createShaderProgram(gl, vsSource, fsSource) {

        const vs = this.createShader(gl, gl.VERTEX_SHADER, vsSource);
        const fs = this.createShader(gl, gl.FRAGMENT_SHADER, fsSource);
        const program = this.createProgram(gl, vs, fs);

        return program;
    }
}