function testDo() {
    console.log("testDo() ...\n");
}
export class SimpleRenderer {

    constructor(canvas) {

        this.glCtx = null;
        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.prog_0 = null;
        this.vao_0 = null;
        this.prog_tex = null;
        this.vao_tex = null;
        this.textures = [];
        this.rcmsTotal = 0;
    }
    initialize(gl) {

        this.glCtx = gl;

        testDo();

        console.log("SimpleRenderer::initialize() ...\n");

        this.vertSource = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;

uniform mat3 u_matrix;

void main() {
    vec3 pos = u_matrix * vec3(a_pos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;

        this.fragSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    //outColor = vec4(1.0, 0.8, 0.8, 1.0);
    outColor = u_color;
}
`;

        this.vertTexSource = `#version 300 es
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

        this.fragTexSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
in vec2 v_uv;
uniform sampler2D u_tex;
out vec4 fragColor;
void main() {
    fragColor = texture(u_tex, v_uv) * u_color;
}
`;
        this.initRender(gl);
    }


    initRender(gl) {

        var program = this.createShaderProgram(gl, this.vertSource, this.fragSource);
        let matrixLoc = gl.getUniformLocation(program, "u_matrix");
        let colorLoc = gl.getUniformLocation(program, "u_color");
        this.prog_0 = { program: program, matrixLoc: matrixLoc, colorLoc: colorLoc };
        this.vao_0 = this.createVAO(gl, program, this.getVerts());

        program = this.createShaderProgram(gl, this.vertTexSource, this.fragTexSource);
        matrixLoc = gl.getUniformLocation(program, "u_matrix");
        colorLoc = gl.getUniformLocation(program, "u_color");
        let texLoc = gl.getUniformLocation(program, "u_tex");
        this.prog_tex = { program: program, matrixLoc: matrixLoc, colorLoc: colorLoc, texLoc: texLoc };
        this.vao_tex = this.createTexVAO(gl, program, this.getVertsWithUV());
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

    normlizeViewSize() {

        let pw = window.innerWidth;
        let ph = window.innerHeight;
        const dpr = window.devicePixelRatio || 1;
        pw = Math.round(pw * dpr);
        ph = Math.round(ph * dpr);
        return [pw, ph];
    }

    runBegin(gl, vw, vh) {

        this.ctxWidth = vw;
        this.ctxHeight = vw;
        gl.clearColor(0.95, 0.95, 0.95, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, vw, vh);
    }
    
    run(gl, vw, vh, dataF32) {

        let rcmsTotal = this.rcmsTotal;

        if (rcmsTotal < 1) {
            return;
        }

        this.runBegin(gl, vw, vh);

        // console.log("vw, vh: ", vw, vh);
        // console.log("vao_0.program: ", vao_0.program);
        // console.log("vao_0.vao: ", vao_0.vao);
        gl.useProgram(this.prog_0.program);
        gl.bindVertexArray(this.vao_0.vao);

        let matvs = new Float32Array([1, 0, 0, 0, 1, 0, 0, 0, 1]);
        matvs = dataF32.subarray(0, 9);

        // let f32Str = "";
        // for (let i = 0; i < matvs.length; i++) {
        //     f32Str += (i > 0 ? "," : "") + matvs[i];
        // }
        // console.log("matvs.length: ", matvs.length);
        // console.log("f32Str: ", f32Str);

        let color = new Float32Array([0.0, 0.6, 0.0, 1.0]);
        gl.uniform4fv(this.prog_0.colorLoc, color);
        gl.uniformMatrix3fv(this.prog_0.matrixLoc, false, matvs, 0, 9);
        gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

        // matvs = dataF32.subarray(9, 18);
        // color = new Float32Array([0.0, 0.6, 0.6, 1.0]);
        // gl.uniform4fv(prog_0.colorLoc, color);
        // gl.uniformMatrix3fv(prog_0.matrixLoc, false, matvs, 0, 9);
        // gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

        if (this.textures.length > 0) {
            let tex = this.textures[0];
            // console.log("xxxxxxxxx tex: ", tex);
            gl.useProgram(this.prog_tex.program);
            gl.bindVertexArray(this.vao_tex.vao);

            matvs = dataF32.subarray(9, 18);
            gl.uniformMatrix3fv(this.prog_tex.matrixLoc, false, matvs, 0, 9);

            color = new Float32Array([0.6, 0.0, 0.6, 1.0]);
            gl.uniform4fv(this.prog_tex.colorLoc, color);
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, tex);
            gl.uniform1i(this.prog_tex.texLoc, 0);
            gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
        }

        gl.useProgram(this.prog_0.program);
        gl.bindVertexArray(this.vao_0.vao);
        color = new Float32Array([0.0, 0.6, 0.6, 1.0]);
        gl.uniform4fv(this.prog_0.colorLoc, color);

        for (let i = 2; i < this.rcmsTotal; ++i) {
            matvs = dataF32.subarray(i * 9, (i + 1) * 9);
            gl.uniformMatrix3fv(this.prog_0.matrixLoc, false, matvs, 0, 9);
            gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
        }

    }
    runEnd(gl) {

    }
}