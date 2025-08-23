"use strict";

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { getIndicesWithSegN, getVertsWithVEOSegN, getVertsWithUV } from './GeomUtils.js';
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

export class SimpleROScene {

    constructor() {

        this.glCtx = null;

        this.dirty = true;

        this.batchTotal = 4;
        this.batchUnit = new BatchROUnit(this.batchTotal);
        this.batchUnit.initialize();
    }
    initialize(gl) {

        this.glCtx = gl;

        this.screenBgColorUnit = new ROUnit();
        this.screenBgColorUnit.initialize();

        this.mvpUnit = new MVPROUnit();
        this.mvpUnit.initialize({ scaleX: 100, scaleY: 100 });

        this.testUnits = [];
        this.initScene(gl);
    }

    initScene(gl) {

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
        ShaderBuilder.createShaderUnit(this.screenBgColorUnit.shader, gl, vertSourceScreenV3, fragSource, shaderDescArr);
        program = this.screenBgColorUnit.shader.program;
        VertexBuilder.createVAO(this.screenBgColorUnit.vertex, gl, program, getVertsWithVEOSegN(1), [2], [2 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.screenBgColorUnit.vertex, gl, getIndicesWithSegN(1));

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


        this.mvpTexUnit = new MVPTexROUnit();
        this.mvpTexUnit.initialize({ scaleX: 200, scaleY: 200 });

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

        this.mvpTexUnit.setXY(360, 200);
        this.mvpTexUnit.setRGBAWithNumberArr([1, 1, 1, 0.5]);
        this.mvpTexUnit.setTexturesWithUrls(['./assets/box.jpg'], this);

        let texUnit0 = this.mvpTexUnit.clone();
        texUnit0.setRGBAWithNumberArr([1, 1, 1, 1]);
        texUnit0.setTexturesWithUrls(['./assets/letterA.png'], this);
        texUnit0.setXY(560, 150);

        let mvpUnit0 = this.mvpUnit.clone();
        mvpUnit0.setRGBAWithNumberArr([1, 0.5, 0.5]);

        let mvpUnit1 = mvpUnit0.clone();
        mvpUnit1.setRGBAWithNumberArr([0.2, 0.5, 0.1]);
        mvpUnit1.setScaleXY(200, 200);
        mvpUnit1.setXY(60, 360);

        let mvpUnit1A = mvpUnit1.clone();
        mvpUnit1A.setRGBAWithNumberArr([0.9, 0.9, 0]);
        mvpUnit1A.setScaleXY(200, 1);
        mvpUnit1A.setXY(60, 400);

        let mvpUnit1B = mvpUnit1.clone();
        mvpUnit1B.setRGBAWithNumberArr([0.9, 0.9, 0]);
        mvpUnit1B.setScaleXY(200, 1);
        mvpUnit1B.setXY(60, 435);

        this.testUnits = [this.mvpUnit, mvpUnit0, mvpUnit1, mvpUnit1A, mvpUnit1B, this.mvpTexUnit, texUnit0, ];
    }
    runBegin() {
        
    }
    run() {
        
    }
    runEnd() {
        this.dirty = false;
    }
}