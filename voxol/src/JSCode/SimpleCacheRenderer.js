"use strict";

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';

const vertSourceScreenV3 = `#version 300 es
precision highp float;

layout(location = 0) in vec2 a_pos;
uniform mat3 u_objMat;
void main() {
    vec3 pos = u_objMat * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
`;
const vertSourceMVPV3 = `#version 300 es
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

const fragSource = `#version 300 es
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    outColor = u_color;
}
`;

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

    if (n > 1) {
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
    } else {
        let verts = new Float32Array([
            x, y,
            x + w, y,
            x + w, y + h,
            x, y + h]);
        return verts;
    }
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

export class SimpleCacheDrawer {

    constructor() {

        this.moduleIns = null;
        this.glCtx = null;
        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.batchTotal = 4;

        this.drewTotal = 0;

        this.batchUnit = new BatchROUnit(this.batchTotal);
        this.batchUnit.initialize();

        this.screenColorUnit = new ROUnit();
        this.screenColorUnit.initialize();

        this.mvpUnit = new MVPROUnit();
        this.mvpUnit.initialize();
    }

    initialize(moduleIns, gl, vw, vh) {


        this.moduleIns = moduleIns;
        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        console.log("SimpleCacheDrawer::initialize() ...\n");

        this.initRender(gl);
    }


    initRender(gl) {

        let segN = this.batchTotal;

        let shaderDescArr = null;
        let program = null;

        shaderDescArr = [{ name: 'u_transforms[0]', type: 'mat3[]' }, { name: 'u_colors[0]', type: 'vec4[]' }];
        ShaderBuilder.createShaderUnit(this.batchUnit.shader, gl, getVertSourceV3SegN(segN), getFragSourceSegN(segN), shaderDescArr);
        program = this.batchUnit.shader.program;
        VertexBuilder.createVAO(this.batchUnit.vertex, gl, program, getVertsWithVEOSegN(segN), [3], [3 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.batchUnit.vertex, gl, getIndicesWithSegN(segN));

        shaderDescArr = [{ name: 'u_objMat', type: 'mat3' }, { name: 'u_color', type: 'vec4' }];
        ShaderBuilder.createShaderUnit(this.screenColorUnit.shader, gl, vertSourceScreenV3, fragSource, shaderDescArr);
        program = this.screenColorUnit.shader.program;
        VertexBuilder.createVAO(this.screenColorUnit.vertex, gl, program, getVertsWithVEOSegN(1), [2], [2 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.screenColorUnit.vertex, gl, getIndicesWithSegN(1));

        shaderDescArr = [
            { name: 'u_objMat', type: 'mat3' },
            { name: 'u_viewMat', type: 'mat3' },
            { name: 'u_projMat', type: 'mat3' },
            { name: 'u_color', type: 'vec4' }
        ];
        ShaderBuilder.createShaderUnit(this.mvpUnit.shader, gl, vertSourceMVPV3, fragSource, shaderDescArr);
        program = this.mvpUnit.shader.program;
        VertexBuilder.createVAO(this.mvpUnit.vertex, gl, program, getVertsWithVEOSegN(1), [2], [2 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.mvpUnit.vertex, gl, getIndicesWithSegN(1));

        let viewTransDesc = this.moduleIns.viewTransDesc;
        this.mvpUnit.viewMatData = viewTransDesc.viewF32;
        this.mvpUnit.projMatData = viewTransDesc.projF32;
        // mvpUnit

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

        this.screenColorUnit.bind(gl);
        this.screenColorUnit.draw(gl);
        //this.mvpUnit

        
        this.mvpUnit.bind(gl);
        this.mvpUnit.draw(gl);

        this.batchUnit.bind(gl);

        for (; ;) {
            if (drewTot >= 180000) {
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
                    this.batchUnit.parse(drawIndex, cmdIndex, dataU32, dataF32);
                    break;
                default:
                    break;
            }
            cmdIndex += descSize;

            drawIndex++;
            if (drawIndex >= matTot) {
                drewTot += matTot;
                this.batchUnit.draw(gl);
                drawIndex = 0;
            }
        }

        if (drewTot != this.drewTotal) {
            this.drewTotal = drewTot;
            console.log("drewTotal: ", this.drewTotal);
        }
    }
    runEnd() {
    }
}