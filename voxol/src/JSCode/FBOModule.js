"use strict";

import { Mat33 } from './Mat33.js';
import { MVPTexROUnit } from './ROUnitModule.js';

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
        let gl = this.glCtx;
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
    }

    bindTexture(fboTex, width, height) {

        let gl = this.glCtx;

        this.fboTex = fboTex == undefined ? gl.createTexture() : fboTex;

        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);

        if (fboTex == undefined) {
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        }

        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);

        gl.bindTexture(gl.TEXTURE_2D, null);
    }

    unbindFBO() {
        let gl = this.glCtx;
        if (this.fboTex) {
            gl.bindTexture(gl.TEXTURE_2D, this.fboTex);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
            gl.generateMipmap(gl.TEXTURE_2D);
            gl.bindTexture(gl.TEXTURE_2D, null);
        }
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
}

export class TileUnit {
    constructor(width, height) {

        this.x = 0;
        this.y = 0;
        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

        this.dirty = true;

        this.wscRenderer = null;
        this.texture = null;
        this.roUnit = new MVPTexROUnit();
        this.fboUnit = null;

        this.viewMat3 = new Mat33();
            // viewMat3.data.set(wscCtx.viewF32);
            // viewMat3.setXY(-this.x, -this.y);
        this.projMat3 = new Mat33();
        this.projMat3.ortho(this.width, this.height);
    }
    setXY(tx, ty) {
        this.x = tx;
        this.y = ty;
    }
    initialize(wscRenderer, fboUnit, srcRoUnit) {

        this.wscRenderer = wscRenderer;
        this.fboUnit = fboUnit;

        this.roUnit.shader = srcRoUnit.shader;
        this.roUnit.vertex = srcRoUnit.vertex;
        this.roUnit.initialize({ scaleX: this.width, scaleY: this.height, texturesNumber: 1 });
    }

    buildBegin() {

        let gl = this.fboUnit.glCtx;

        this.fboUnit.bindFBO(gl);
        this.fboUnit.bindTexture(this.texture, this.width, this.height);

        gl.clearColor(0.55, 0.95, 0.55, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, this.width, this.height);
        this.wscRenderer.dirty = true;
    }
    buildDraw(ctx) {

        let gl = this.fboUnit.glCtx;
        let wscRenderer = this.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;

        if (ctx == undefined) {

            let viewMat3 = this.viewMat3;
            viewMat3.data.set(wscCtx.viewF32);
            viewMat3.setXY(-this.x, -this.y);
            let projMat3 = this.projMat3;
            ctx = { viewF32: viewMat3.data, projF32: projMat3.data };
        }

        this.texture = this.fboUnit.fboTex;
        console.log("TileUnit::build() A ctx: ", ctx);
        wscRenderer.draw(ctx);
        console.log("TileUnit::build() B ...");

        this.roUnit.setTextures([this.texture]);
    }
    buildEnd() {
        this.fboUnit.unbindFBO();
        this.wscRenderer.dirty = false;
    }

    /// check tile area dirty yes or no
    tileDirtyCheck() {
        return this.dirty;
    }
    build() {
        let tileTirty = this.tileDirtyCheck();
        if (tileTirty) {
            this.buildBegin();
            this.buildDraw();
            this.buildEnd();
            this.dirty = false;
        }
        return tileTirty;
    }

    draw(ctx) {

        let gl = this.fboUnit.glCtx;

        // let wscRenderer = this.wscRenderer;
        // let moduleIns = wscRenderer.moduleIns;
        // let wscCtx = moduleIns.viewTransDesc;
        // ctx = ctx == undefined ? wscCtx : ctx;

        let unit = this.roUnit;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
    }
    drawTest(ctx) {

        let gl = this.fboUnit.glCtx;

        this.build();

        let wscRenderer = this.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;
        ctx = ctx == undefined ? wscCtx : ctx;

        wscRenderer.runBegin();

        let unit = this.roUnit;
        if (unit && unit.enabled) {
            unit.bind(gl, ctx);
            unit.draw(gl, ctx);
        }
    }
    update() {

        let gl = this.fboUnit.glCtx;

        let unit = this.roUnit;
        unit.setXY(this.x, this.y);
        unit.setScaleXY(this.width, this.height);
    }

}