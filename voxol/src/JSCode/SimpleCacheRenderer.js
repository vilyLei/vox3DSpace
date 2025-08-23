"use strict";

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { TextureBuilder } from './TextureModule.js';
import { getIndicesWithSegN, getVertsWithVEOSegN, getVertsWithUV} from './GeomUtils.js';
import { MVPTexROUnit, BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';
import {
    fragPreMultAlphaTexSource,
    vertTexMVPSource,
    vertSourceMVPV3,
    vertSourceScreenV3,
    fragTexSource,
    fragSource,
    getVertSourceMVPV3SegN,
    getVertSourceV3SegN,
    getFragSourceSegN
} from './ShaderCodes.js';


export function printWith9Number(numArr, index) {

    let f32Str = "";
    for (let i = 0; i < 9; i++) {
        f32Str += numArr[index + i];
        f32Str += ((i + 1) % 3) == 0 ? ",\n" : (i < 8 ? "," : "");
    }
    console.log("array9Numbers:");
    console.log(f32Str);
}


export class SimpleCacheDrawer {

    constructor() {

        this.moduleIns = null;
        this.glCtx = null;
        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.dirty = true;
        this.batchTotal = 4;

        this.drewTotal = 0;

        this.batchUnit = new BatchROUnit(this.batchTotal);
        this.batchUnit.initialize();

        this.screenColorUnit = new ROUnit();
        this.screenColorUnit.initialize();

        this.mvpUnit = new MVPROUnit();
        this.mvpUnit.initialize({ scaleX: 100, scaleY: 100 });
        this.mvpUnit0 = this.mvpUnit.clone();
        this.mvpUnit0.setRGBAWithNumberArr([1, 0.5, 0.5]);

    }

    initialize(moduleIns, gl, vw, vh) {


        let thisRef = this;

        this.moduleIns = moduleIns;
        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        console.log("SimpleCacheDrawer::initialize() ...\n");

        let texUrls = ['./assets/box.jpg'];
        // let texUrls = ['./assets/letterA.png'];
        this.mvpTexUnit = new MVPTexROUnit();
        this.mvpTexUnit.initialize({ scaleX: 200, scaleY: 200, texturesNumber: texUrls.length });
        this.mvpTexUnit.setXY(360, 200);
        this.mvpTexUnit.setRGBAWithNumberArr([1,1,1, 0.5]);
        this.mvpTexUnit.setTexturesWithUrls(texUrls, this);
        this.initRender(gl);
    }


    initRender(gl) {

        let segN = this.batchTotal;

        let program = null;
        let shaderDescArr = null;
        let textureDescArr = null;

        // shaderDescArr = [{ name: 'u_transforms[0]', type: 'mat3[]' }, { name: 'u_colors[0]', type: 'vec4[]' }];
        // ShaderBuilder.createShaderUnit(this.batchUnit.shader, gl, getVertSourceV3SegN(segN), getFragSourceSegN(segN), shaderDescArr);
        shaderDescArr = [
            { name: 'u_transforms[0]', type: 'mat3[]' },
            { name: 'u_viewMat', type: 'mat3' },
            { name: 'u_projMat', type: 'mat3' },
            { name: 'u_colors[0]', type: 'vec4[]' }
        ];
        ShaderBuilder.createShaderUnit(this.batchUnit.shader, gl, getVertSourceMVPV3SegN(segN), getFragSourceSegN(segN), shaderDescArr);
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


        shaderDescArr = [
            { name: 'u_objMat', type: 'mat3' },
            { name: 'u_viewMat', type: 'mat3' },
            { name: 'u_projMat', type: 'mat3' },
            { name: 'u_color', type: 'vec4' }
        ];
        textureDescArr = [
            { name: 'u_tex0', type: 'texture2D' }
        ];
        ShaderBuilder.createShaderUnit(this.mvpTexUnit.shader, gl, vertTexMVPSource, fragPreMultAlphaTexSource, shaderDescArr, textureDescArr);
        // ShaderBuilder.createShaderUnit(this.mvpTexUnit.shader, gl, vertTexMVPSource, fragTexSource, shaderDescArr, textureDescArr);
        program = this.mvpTexUnit.shader.program;
        VertexBuilder.createVAO(this.mvpTexUnit.vertex, gl, program, getVertsWithUV(), [4], [4 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.mvpTexUnit.vertex, gl, getIndicesWithSegN(1));

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

    draw(ctx) {

        let moduleIns = this.moduleIns;
        let gl = this.glCtx;
        ctx = ctx == undefined ? moduleIns.viewTransDesc : ctx;

        let unit = this.screenColorUnit;
        if (unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
        this.drawBatch(ctx);
    }
    drawBatch(ctx) {

        let moduleIns = this.moduleIns;
        let gl = this.glCtx;
        ctx = ctx == undefined ? moduleIns.viewTransDesc : ctx;

        let matTot = this.batchTotal;
        let drawIndex = 0;
        let drewTot = 0;

        let batchEle = moduleIns.batchEleDesc;
        let cmdIndex = batchEle.getElementDataIndex();
        let dataU32 = batchEle.heapU32;
        let dataF32 = batchEle.heapF32;

        let unit = this.batchUnit;


        unit.bind(gl, ctx);

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
                case 0x32:
                    unit.parse(drawIndex, cmdIndex, dataU32, dataF32);
                    break;
                default:
                    break;
            }
            cmdIndex += descSize;

            drawIndex++;
            if (drawIndex >= matTot) {
                drewTot += matTot;
                unit.draw(gl, ctx);
                drawIndex = 0;
            }
        }

        if (drewTot != this.drewTotal) {
            this.drewTotal = drewTot;
            console.log("drewTotal: ", this.drewTotal);
        }

        unit = this.mvpUnit;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
        unit = this.mvpUnit0;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }

        unit = this.mvpTexUnit;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
    }
    runEnd() {
    }
}