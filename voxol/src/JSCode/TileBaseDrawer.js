"use strict";

import { ShaderBuilder } from './ShaderModule.js';
import { VertexBuilder } from './VertexModule.js';
import { getIndicesWithSegN, getVertsWithVEOSegN, getVertsWithUV } from './GeomUtils.js';
import { MVPTexROUnit, BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';
import {
    fragPreMultAlphaFlipYTexSource,
    vertTexMVPSource,
} from './ShaderCodes.js';

import { TileParams, FBOUnit, TileGrid, TileUnit } from './FBOModule.js';

function calcCeilOfTwoLevel(value) {
    return Math.ceil(Math.log(value) / Math.LN2);
}
function calcFloorOfTwoLevel(value) {
    return Math.floor(Math.log(value) / Math.LN2);
}
function calcCeilPowerOfTwo(value) {
    return Math.pow(2, Math.ceil(Math.log(value) / Math.LN2));
}
export class TileBaseDrawer {
    constructor() {

        this.glCtx = null;
        this.wscRenderer = null;
        this.fboIns = null;
        this.texROUnit = new MVPTexROUnit();

        // this.tile0 = null;
        // this.tile1 = null;

        this.tilesRN = 3;
        this.tilesCN = 3;
        this.tiles = new Array(this.tilesRN * this.tilesCN);
    }
    initialize(gl, wscRenderer) {

        this.glCtx = gl;
        this.wscRenderer = wscRenderer;
        this.fboIns = new FBOUnit();
        this.fboIns.initialize(gl);

        this.buildCfg();

        // this.tile0 = new TileUnit(256, 256);
        // this.tile0.initialize(wscRenderer, this.fboIns, this.texROUnit);
        // this.tile1 = new TileUnit(256, 256);
        // this.tile1.initialize(wscRenderer, this.fboIns, this.texROUnit);
        // this.tile1.setXY(256, 0);
        // this.tile1.update();

        let index = 0;
        for (let i = 0; i < this.tilesRN; ++i) {
            let py = i * 256;
            for (let j = 0; j < this.tilesRN; ++j) {
                let px = j * 256;
                let tile = new TileGrid(px, py, 256, 256);
                tile.initialize(wscRenderer, this.fboIns, this.texROUnit);
                this.tiles[index] = tile;
                index++;
            }
        }

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
        let rstatus = wscCtx.status;
        let fbo = this.fboIns;
        fbo.bindTexTimes = 0;

        rstatus.reset();

        // for test
        // this.tile0.drawTest();
        // this.tile1.drawTest();
        // this.tile0.build();
        // this.tile1.build();
        // wscRenderer.runBegin();
        // this.tile0.draw(wscCtx);
        // this.tile1.draw(wscCtx);

        let zoom = moduleIns.getZoom();
        // zoom = Math.round(zoom * 1000) / 1000;

        let defaultViewSize = TileParams.defaultViewSize;

        let sizeValue = zoom * defaultViewSize;
        let viewLevel = calcCeilOfTwoLevel(sizeValue) - 1;
        if (viewLevel < TileParams.defaultViewLevel)
            viewLevel = TileParams.defaultViewLevel;

        console.log(`TileBaseDrawer::draw(), zoom = ${zoom}, viewLevel = ${viewLevel}, (2 << viewLevel)=${2 << viewLevel}`);

        let worldLevel = calcFloorOfTwoLevel(defaultViewSize / zoom) - 1;
        if (worldLevel < TileParams.defaultViewLevel)
            worldLevel = TileParams.defaultViewLevel;

        console.log(`TileBaseDrawer::draw(), worldLevel = ${worldLevel}, viewLevel = ${viewLevel}, (2 << worldLevel)=${2 << worldLevel}`);

        let tiles = this.tiles;
        let len = tiles.length;
        
        for (let i = 0; i < len; ++i) {
            const t = tiles[i];
            if (t.checkDrawing()) {
                t.setWorldLevel(worldLevel);
                t.setViewLevel(viewLevel);
                t.build();
            }
        }


        wscRenderer.runBegin();

        rstatus.drawTimes = 0;

        for (let i = 0; i < len; ++i) {
            const t = tiles[i];
            if (t.checkDrawing()) {
                tiles[i].draw(wscCtx);
            }
        }
        console.log(`draw(), bindTexTimes = ${fbo.bindTexTimes}`);
        rstatus.print();

    }

}