"use strict";

import { Mat33 } from './Mat33.js';
import { ViewBounds } from './CGeomBase.js';
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

        // defaut value: world space coordinates
        this.x = x != undefined ? x : 0;
        this.y = y != undefined ? y : 0;

        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

        this.level = 7;

        this.dirty = true;

        this.wscRenderer = null;
        this.texture = null;
        this.fboUnit = null;

        this.viewMat3 = new Mat33();
        this.projMat3 = new Mat33();
        this.projMat3.ortho(this.width, this.height);
        this.viewTransDesc = {
            viewF32: this.viewMat3.data,
            projF32: this.projMat3.data,
            viewWBounds: new ViewBounds(0, 0, 512, 512)
        };
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

        this.wscRenderer.dirty = true;
    }

    buildDraw() {

        let wscRenderer = this.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;
        // 判断当前 tile是否被view world bounds包含, 如果包含了才会实际执行绘制

        let gl = this.fboUnit.glCtx;

        let pw = this.width;
        let ph = this.height;

        let zoom = 1;

        pw *= zoom;
        ph *= zoom;

        let px = this.x * zoom;
        let py = this.y * zoom;

        this.viewMat3.setTo(-px, -py, zoom, zoom);
        this.projMat3.ortho(pw, ph);

        console.log("TileRODesc::buildDraw(), this.viewTransDesc: ", this.viewTransDesc);

        this.fboUnit.bindFBO(gl);
        this.fboUnit.bindTexture(this.texture, pw, ph);
        this.texture = this.fboUnit.fboTex;

        gl.clearColor(0.55, 0.95, 0.55, 1);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, pw, ph);
        // 如果绘制的有实际内容，则这个tile有效，反之无效， 无效了之后这个tile资源就可以释放了
        wscRenderer.draw(this.viewTransDesc);

        this.fboUnit.unbindFBO();
    }

    buildEnd() {
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