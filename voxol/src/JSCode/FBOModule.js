"use strict";

import { Mat33 } from './Mat33.js';
import { MVPTexROUnit, BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';

export class FBOUnit {
    constructor() {
        this.fbo = null;
        this.fboTex = null;
        this.glCtx = null;
    }

    initialize(gl, fbo) {
        this.glCtx = gl;
        this.fbo = fbo == undefined ? gl.createFramebuffer() : fbo;
    }

    bindFBO() {
        let gl = this.fboUnit.glCtx;
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
    }

    bindTexture(fboTex, width, height) {
        let gl = this.fboUnit.glCtx;

        this.fboTex = fboTex == undefined ? gl.createTexture() : fboTex;

        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);

        if (fboTex == undefined) {
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        }

        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);
    }

    unbindFBO() {
        let gl = this.fboUnit.glCtx;
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
}

export class TileUnit {
    constructor(width, height) {

        this.x = 0;
        this.y = 0;
        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

        this.texture = null;
        this.roUnit = new MVPTexROUnit();
        this.fboUnit = null;
    }
    initialize(fboUnit, srcRoUnit) {

        this.fboUnit = fboUnit;

        this.roUnit.shader = srcRoUnit.shader;
        this.roUnit.vertex = srcRoUnit.vertex;
        this.roUnit.initialize({ scaleX: this.width, scaleY: this.height, texturesNumber: 1 });
    }

    buildBegin(wscRenderer) {
        let gl = this.fboUnit.glCtx;
        gl.clearColor(0.75, 0.95, 0.75, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, this.width, this.height);
        wscRenderer.dirty = true;
    }
    build(wscRenderer) {
        let gl = this.fboUnit.glCtx;
        this.fboUnit.bindFBO(gl);
        this.fboUnit.bindTexture(this.texture);
        this.texture = this.fboUnit.fboTex;

        let viewMat3 = new Mat33();
        let projMat3 = new Mat33();
        projMat3.ortho(this.width, this.height);

        let currCtx = { viewF32: viewMat3.data, projF32: projMat3.data };
        wscRenderer.draw(currCtx);

        this.roUnit.setTextures([this.texture]);
    }
    buildEnd(wscRenderer) {
    }

    draw(ctx) {
        let unit = this.roUnit;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
    }
    update() {
        this.roUnit.setXY(this.x, this.y);
        this.roUnit.setScaleXY(this.width, this.height);
    }

}