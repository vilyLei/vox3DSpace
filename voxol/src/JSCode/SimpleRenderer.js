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
    
    getVerts() {
        let x = 0, y = 0, w = 1, h = 1;
        let verts = new Float32Array([
            x, y,
            x + w, y,
            x, y + h,
            x + w, y + h]
        );
        return verts;
    }

    getVertsWithUV() {
        let x = 0, y = 0, w = 1, h = 1;
        let verts = new Float32Array([
            x, y, 0, 0,
            x + w, y, 1, 0,
            x, y + h, 0, 1,
            x + w, y + h, 1, 1
        ]
        );
        return verts;
    }

    createVAO(gl, program, verts) {

        //vertexAttribIPointer(index, size, type, stride, offset)

        const vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        const vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
        gl.bufferData(gl.ARRAY_BUFFER, verts, gl.STATIC_DRAW);

        const posLoc = gl.getAttribLocation(program, "a_pos");
        gl.enableVertexAttribArray(posLoc);
        gl.vertexAttribPointer(posLoc, 2, gl.FLOAT, false, 8, 0);

        return { program: program, vao: vao, locs: [posLoc] };
    }

    createTexVAO(gl, program, verts) {

        //vertexAttribIPointer(index, size, type, stride, offset)

        const vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        const vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
        gl.bufferData(gl.ARRAY_BUFFER, verts, gl.STATIC_DRAW);

        const posLoc = gl.getAttribLocation(program, "a_pos");
        const uvLoc = gl.getAttribLocation(program, "a_uv");
        gl.enableVertexAttribArray(posLoc);
        gl.vertexAttribPointer(posLoc, 2, gl.FLOAT, false, 16, 0);
        gl.enableVertexAttribArray(uvLoc);
        gl.vertexAttribPointer(uvLoc, 2, gl.FLOAT, false, 16, 8);

        return { program: program, vao: vao, locs: [posLoc, uvLoc] };
    }
    
    createTextureFromImage(gl, image) {
        const tex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, tex);

        gl.texImage2D(
            gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA,
            gl.UNSIGNED_BYTE, image
        );

        // 设置贴图参数
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.generateMipmap(gl.TEXTURE_2D);

        return tex;
    }

    loadImageAndCreateTexture(gl, url, callback) {
        const img = new Image();
        img.onload = () => {
            const tex = this.createTextureFromImage(gl, img);
            callback(tex);
        };
        img.src = url;
    }
}