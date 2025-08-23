"use strict";
// import { SimpleCacheDrawer } from './SimpleCacheRenderer.js';

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { getIndicesWithSegN, getVertsWithVEOSegN, getVertsWithUV } from './GeomUtils.js';
import { MVPTexROUnit, BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';
import {
    fragPreMultAlphaFlipYTexSource,
    vertTexMVPSource,
} from './ShaderCodes.js';

import { FBOUnit, TileUnit } from './FBOModule.js';

export class TileBaseDrawer {
    constructor() {

        this.glCtx = null;
        this.wscRenderer = null;
        this.fboIns = null;
        this.texROUnit = new MVPTexROUnit();

        this.tile0 = null;
        this.tile1 = null;
    }
    initialize(gl, wscRenderer) {

        this.glCtx = gl;
        this.wscRenderer = wscRenderer;
        this.fboIns = new FBOUnit();
        this.fboIns.initialize(gl);

        this.buildCfg();

        this.tile0 = new TileUnit(256, 256);
        this.tile0.initialize(wscRenderer, this.fboIns, this.texROUnit);
        this.tile1 = new TileUnit(256, 256);
        this.tile1.initialize(wscRenderer, this.fboIns, this.texROUnit);
        this.tile1.setXY(256, 0);
        this.tile1.update();
    }
    buildCfg() {

        let gl = this.glCtx;

        this.texROUnit.initialize({ scaleX: 128, scaleY: 128, texturesNumber: 1 });
        let shaderDescArr = [
            { name: 'u_objMat', type: 'mat3' },
            { name: 'u_viewMat', type: 'mat3' },
            { name: 'u_projMat', type: 'mat3' },
            { name: 'u_color', type: 'vec4' }
        ];
        let textureDescArr = [
            { name: 'u_tex0', type: 'texture2D' }
        ];
        ShaderBuilder.createShaderUnit(this.texROUnit.shader, gl, vertTexMVPSource, fragPreMultAlphaFlipYTexSource, shaderDescArr, textureDescArr);
        let program = this.texROUnit.shader.program;
        VertexBuilder.createVAO(this.texROUnit.vertex, gl, program, getVertsWithUV(), [4], [4 * 4], ['a_pos']);
        VertexBuilder.createVEO(this.texROUnit.vertex, gl, getIndicesWithSegN(1));
    }
    draw() {
        let wscRenderer = this.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;
        // this.tile0.drawTest();
        // this.tile1.drawTest();

        this.tile0.build();
        this.tile1.build();

        wscRenderer.runBegin();
        
        this.tile0.draw(wscCtx);
        this.tile1.draw(wscCtx);
    }

}