function testDo() {
    console.log("testDo() ...\n");
}
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
    fragColor = texture(u_tex, v_uv) * u_color;
}
`;


function getVerts() {
    let x = 0, y = 0, w = 1, h = 1;
    let verts = new Float32Array([
        x, y,
        x + w, y,
        x, y + h,
        x + w, y + h]
    );
    return verts;
}

function getVertsWithUV() {
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
class BaseFBOIns {
    constructor() {
        this.fbo = null;
        this.fboTex = null;
    }

    initFBO(gl, width, height) {
        this.fboTex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

        this.fbo = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }

    bindFBO(gl, width, height) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
        gl.viewport(0, 0, width, height);
    }

    unbindFBO(gl, width, height) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        gl.viewport(0, 0, width, height);
    }
}

class MSAAFBOIns {
    constructor() {
        this.msaaFBO = null;
        this.fbo = null;
        this.fboTex = null;
    }
    initFBO(gl, width, height, samples = 2) {

        // 创建多重采样颜色缓冲区（renderbuffer）
        const colorRenderbuffer = gl.createRenderbuffer();
        gl.bindRenderbuffer(gl.RENDERBUFFER, colorRenderbuffer);
        gl.renderbufferStorageMultisample(gl.RENDERBUFFER, samples, gl.RGBA8, width, height);

        // 创建多重采样的 framebuffer
        this.msaaFBO = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.msaaFBO);
        gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER, colorRenderbuffer);

        // 创建目标纹理（非多重采样）
        this.fboTex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

        // 创建一个用于 resolve 的普通 FBO
        this.fbo = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);

        // 清理绑定
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        gl.bindRenderbuffer(gl.RENDERBUFFER, null);
    }

    bindFBO(gl, width, height) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.msaaFBO);
        gl.viewport(0, 0, width, height);
    }

    unbindFBO(gl, width, height) {

        gl.bindFramebuffer(gl.READ_FRAMEBUFFER, this.msaaFBO);
        gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, this.fbo);
        gl.blitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            gl.COLOR_BUFFER_BIT,
            gl.NEAREST
        );

        // 恢复默认帧缓冲
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        gl.viewport(0, 0, width, height);
    }
}
export class SimpleRenderer {

    constructor() {

        this.dataF32 = null;
        this.glCtx = null;

        this.fboIns = new BaseFBOIns();

        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.prog_0 = null;
        this.vao_0 = null;
        this.prog_tex = null;
        this.vao_tex = null;
        this.textures = [];
        this.rcmsTotal = 0;
    }
    initialize(gl, vw, vh) {

        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        testDo();

        console.log("SimpleRenderer::initialize() ...\n");

        this.initRender(gl);
    }


    initRender(gl) {

        var program = this.createShaderProgram(gl, vertSource, fragSource);
        let matrixLoc = gl.getUniformLocation(program, "u_matrix");
        let colorLoc = gl.getUniformLocation(program, "u_color");
        this.prog_0 = { program: program, matrixLoc: matrixLoc, colorLoc: colorLoc };
        this.vao_0 = this.createVAO(gl, program, getVerts());

        program = this.createShaderProgram(gl, vertTexSource, fragTexSource);
        matrixLoc = gl.getUniformLocation(program, "u_matrix");
        colorLoc = gl.getUniformLocation(program, "u_color");
        let texLoc = gl.getUniformLocation(program, "u_tex");
        this.prog_tex = { program: program, matrixLoc: matrixLoc, colorLoc: colorLoc, texLoc: texLoc };
        this.vao_tex = this.createTexVAO(gl, program, getVertsWithUV());

        this.fboIns.initFBO(gl, 512, 512);
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

    setCtxSize(vw, vh) {
        this.ctxWidth = vw;
        this.ctxHeight = vh;
    }
    runBegin() {

        let gl = this.glCtx;
        let vw = this.ctxWidth;
        let vh = this.ctxHeight;
        gl.clearColor(0.95, 0.95, 0.95, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, vw, vh);
    }

    run(dataF32) {

        let rcmsTotal = this.rcmsTotal;

        if (rcmsTotal < 1 || this.glCtx == null) {
            return;
        }

        this.runBegin();

        let gl = this.glCtx;

        dataF32 = dataF32 != null ? dataF32 : this.dataF32;
        this.dataF32 = dataF32;

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

    draw(cmdIndex, dataU32, dataF32) {

        // console.log("vw, vh: ", vw, vh);
        // console.log("vao_0.program: ", vao_0.program);
        // console.log("vao_0.vao: ", vao_0.vao);



        let gl = this.glCtx;
        for (;;) {
            let cmd = dataU32[cmdIndex];
            if (cmd == 0) {
                // console.log("drawing cmd exec end !!!");
                break;
            }
            let descSize = dataU32[cmdIndex + 1];
            //descSize
            switch (cmd) {
                case 0x33:
                    {
                        // console.log("drawing a rect ...");
                        let f32Index = cmdIndex + 3;
                        let matvs = dataF32.subarray(f32Index, f32Index + 9);
                        let colorU32 = dataU32[cmdIndex + 2];
                        // console.log("colorU32: ", colorU32.toString(16), ", b: ", (colorU32 & 0xff), (colorU32 & (0xff << 8)).toString(16));
                        gl.useProgram(this.prog_0.program);
                        gl.bindVertexArray(this.vao_0.vao);

                        let r = ((colorU32 >> 16) & 0xff) / 255.0;
                        let g = ((colorU32 >> 8) & 0xff) / 255.0;
                        let b = (colorU32 & 0xff) / 255.0;
                        let a = ((colorU32 >> 24) & 0xff) / 255.0;

                        let color = new Float32Array([r, g, b, a]);
                        gl.uniform4fv(this.prog_0.colorLoc, color);
                        gl.uniformMatrix3fv(this.prog_0.matrixLoc, false, matvs, 0, 9);
                        gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
                    }
                    break;
                default:
                    break;
            }
            cmdIndex += descSize;
        }

    }
    runEnd() {
    }
}