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

class TileRODesc {
    constructor(x, y, width, height) {

        this.x = x != undefined ? x : 0;
        this.y = y != undefined ? y : 0;
        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

        this.level = 8;

        this.dirty = true;

        this.wscRenderer = null;
        this.texture = null;
        this.fboUnit = null;

        this.viewMat3 = new Mat33();
        this.projMat3 = new Mat33();
        this.projMat3.ortho(this.width, this.height);
    }
    setLevel(level) {

        if (this.level == level)
            return;

        let gl = this.fboUnit.glCtx;
        if (this.texture) {
            gl.deleteTexture(this.texture);
        }
        this.level = level;
        this.dirty = true;

    }

    setXY(tx, ty) {
        this.x = tx;
        this.y = ty;
    }
    setSize(pw, ph) {
        this.width = pw;
        this.height = ph;
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

            let zoom = 1;
            let viewMat3 = this.viewMat3;
            viewMat3.data.set(wscCtx.viewF32);
            viewMat3.setXY(-this.x, -this.y, zoom, zoom);
            let projMat3 = this.projMat3;
            ctx = { viewF32: viewMat3.data, projF32: projMat3.data };
        }

        this.texture = this.fboUnit.fboTex;
        wscRenderer.draw(ctx);
    }

    buildEnd() {

        this.fboUnit.unbindFBO();
        this.wscRenderer.dirty = false;
        this.dirty = false;
    }
}
export class TileUnit {
    constructor(x, y, width, height) {

        this.roDesc = new TileRODesc(x, y, width, height);
        this.roUnit = new MVPTexROUnit();
    }
    setLevel(level) {

        if (this.level == level)
            return;

        this.level = level;
        this.dirty = true;

    }
    initialize(wscRenderer, fboUnit, srcRoUnit) {

        let desc = this.roDesc;
        desc.wscRenderer = wscRenderer;
        desc.fboUnit = fboUnit;

        this.roUnit.shader = srcRoUnit.shader;
        this.roUnit.vertex = srcRoUnit.vertex;
        this.roUnit.initialize({ scaleX: desc.width, scaleY: desc.height, texturesNumber: 1 });
        this.roUnit.setXY(desc.x, desc.y);
    }

    /// check tile area dirty yes or no
    tileDirtyCheck() {
        let desc = this.roDesc;
        return desc.dirty;
    }
    build() {
        let desc = this.roDesc;
        let tileTirty = this.tileDirtyCheck();
        if (tileTirty) {
            desc.buildBegin();
            desc.buildDraw();
            desc.buildEnd();
            this.roUnit.setTextures([desc.texture]);
        }
        return tileTirty;
    }

    draw(ctx) {

        let desc = this.roDesc;
        let gl = desc.fboUnit.glCtx;

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

        let desc = this.roDesc;
        let gl = desc.fboUnit.glCtx;

        this.build();

        let wscRenderer = desc.wscRenderer;
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

        let desc = this.roDesc;
        let gl = desc.fboUnit.glCtx;

        let unit = this.roUnit;
        unit.setXY(this.x, this.y);
        unit.setScaleXY(desc.width, desc.height);
    }

}