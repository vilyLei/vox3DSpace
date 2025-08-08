"use strict";

function getVertSourceV3SegN(n) {

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
function getFragSourceSegN(n) {
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

export function printWith9Number(numArr, index) {

    let f32Str = "";
    for (let i = 0; i < 9; i++) {
        f32Str += numArr[index + i];
        f32Str += ((i + 1) % 3) == 0 ? ",\n" : (i < 8 ? "," : "");
    }
    console.log("array9Numbers:");
    console.log(f32Str);
}

function getVertsWithVEOSegN(n) {

    let x = 0, y = 0, w = 1, h = 1;

    let verts = new Float32Array(n * 12);
    for (let i = 0; i < n; ++i) {
        // transI 用于指定transform矩阵的序号
        let transI = i;
        verts.set([
            x, y, transI,
            x + w, y, transI,
            x + w, y + h, transI,
            x, y + h, transI], i * 12);
    }
    return verts;
}
function getIndicesWithSegN(n) {


    const indices = new Uint16Array(n * 6);
    for (let i = 0; i < n; ++i) {
        let baseI = i * 4;
        indices.set([
            baseI, baseI + 1, baseI + 2,
            baseI + 2, baseI + 3, baseI], i * 6);
    }
    return indices;
}

export class BatchDrawer {

    constructor() {

        this.dataF32 = null;
        this.glCtx = null;

        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.prog_0 = null;
        this.vao_0 = null;
        this.rcmsTotal = 0;
        this.batchTotal = 4;
        
        this.drewTotal = 0;
    }

    initialize(gl, vw, vh) {

        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        console.log("BatchDrawer::initialize() ...\n");

        this.initRender(gl);
    }


    initRender(gl) {

        let segN = this.batchTotal;
        var program = this.createShaderProgram(gl, getVertSourceV3SegN(segN), getFragSourceSegN(segN));
        let matrixLoc = gl.getUniformLocation(program, "u_transforms[0]");
        let colorLoc = gl.getUniformLocation(program, "u_colors[0]");
        this.prog_0 = { program: program, matrixLoc: matrixLoc, colorLoc: colorLoc };
        this.vao_0 = this.createVAO(gl, program, getVertsWithVEOSegN(segN), 3, 3 * 4);

        this.indices_0 = getIndicesWithSegN(segN);
        this.veo_0 = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.veo_0);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.indices_0, gl.STATIC_DRAW);

    }

    createShader(gl, type, source) {
        console.log(source);
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

    createVAO(gl, program, verts, compSize, strideBytes) {

        if (compSize == undefined)
            compSize = 2;
        if (strideBytes == undefined)
            strideBytes = 8;

        const vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        const vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
        gl.bufferData(gl.ARRAY_BUFFER, verts, gl.STATIC_DRAW);

        const posLoc = gl.getAttribLocation(program, "a_pos");
        gl.enableVertexAttribArray(posLoc);
        gl.vertexAttribPointer(posLoc, compSize, gl.FLOAT, false, strideBytes, 0);

        return { program: program, vao: vao, locs: [posLoc] };
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

    draw(cmdIndex, dataU32, dataF32) {

        // console.log("vw, vh: ", vw, vh);
        // console.log("vao_0.program: ", vao_0.program);
        // console.log("vao_0.vao: ", vao_0.vao);


        let gl = this.glCtx;
        let matTot = this.batchTotal;
        let drawIndex = 0;
        let drewTot = 0;

        let transData = new Float32Array(matTot * 9);
        let colorData = new Float32Array(matTot * 4);
        for (; ;) {
            if(drewTot >= 180000) {
                break;
            }
            let cmd = dataU32[cmdIndex];
            if (cmd == 0) {
                // console.log("drawing cmd exec end !!!");
                break;
            }
            let descSize = dataU32[cmdIndex + 1];
            switch (cmd) {
                case 0x33:
                    {
                        let f32Index = cmdIndex + 3;
                        let matvs = dataF32.subarray(f32Index, f32Index + 9);
                        transData.set(matvs, drawIndex * 9);
                        let colorU32 = dataU32[cmdIndex + 2];
                        let a = ((colorU32 >> 24) & 0xff) / 255.0;
                        let r = ((colorU32 >> 16) & 0xff) / 255.0;
                        let g = ((colorU32 >> 8) & 0xff) / 255.0;
                        let b = (colorU32 & 0xff) / 255.0;
                        colorData.set([r, g, b, a], drawIndex * 4);
                    }
                    break;
                default:
                    break;
            }
            cmdIndex += descSize;

            ///*
            drawIndex++;
            if (drawIndex >= matTot) {

                drewTot += matTot;
                gl.useProgram(this.prog_0.program);
                gl.bindVertexArray(this.vao_0.vao);
                gl.uniform4fv(this.prog_0.colorLoc, colorData, 0, matTot * 4);
                gl.uniformMatrix3fv(this.prog_0.matrixLoc, false, transData, 0, matTot * 9);

                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.veo_0);
                gl.drawElements(gl.TRIANGLES, this.indices_0.length, gl.UNSIGNED_SHORT, 0);
                drawIndex = 0;
            }
        }

        if(drewTot != this.drewTotal) {
            this.drewTotal = drewTot;
            console.log("drewTotal: ", this.drewTotal);
        }
    }
    runEnd() {
    }
}