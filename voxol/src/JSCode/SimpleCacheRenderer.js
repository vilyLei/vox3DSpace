"use strict";

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { BatchROUnit } from './ROModule.js';

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

export class CacheDrawer {

    constructor() {

        this.dataF32 = null;
        this.glCtx = null;
        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.rcmsTotal = 0;
        this.batchTotal = 4;
        
        this.drewTotal = 0;

        this.roUnit = new BatchROUnit();
    }

    initialize(gl, vw, vh) {

        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        console.log("CacheDrawer::initialize() ...\n");

        this.initRender(gl);
    }


    initRender(gl) {

        let segN = this.batchTotal;
        let shaderDescArr = [{name:'u_transforms[0]', type:'mat4[]'},{name:'u_colors[0]', type:'vec4[]'}];
        ShaderBuilder.createShaderUnit(this.roUnit.shader, gl, getVertSourceV3SegN(segN), getFragSourceSegN(segN), shaderDescArr);

        let program = this.roUnit.shader.program;
        VertexBuilder.createVAO(this.roUnit.vertex, gl, program, getVertsWithVEOSegN(segN), [3], [3 * 4],['a_pos']);
        VertexBuilder.createVEO(this.roUnit.vertex, gl, getIndicesWithSegN(segN));

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

        let gl = this.glCtx;
        let matTot = this.batchTotal;
        let drawIndex = 0;
        let drewTot = 0;

        let transData = new Float32Array(matTot * 9);
        let colorData = new Float32Array(matTot * 4);

        let vertex = this.roUnit.vertex;
        let shader = this.roUnit.shader;

        this.roUnit.bind(gl);
        // gl.useProgram(shader.program);
        // gl.bindVertexArray(vertex.vao);
        // gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vertex.veo);

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

            drawIndex++;
            if (drawIndex >= matTot) {
                drewTot += matTot;
                gl.uniformMatrix3fv(shader.uniforms[0].location, false, transData, 0, matTot * 9);
                gl.uniform4fv(shader.uniforms[1].location, colorData, 0, matTot * 4);
                // gl.drawElements(gl.TRIANGLES, vertex.indices.length, gl.UNSIGNED_SHORT, 0);
                
                this.roUnit.draw(gl);
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