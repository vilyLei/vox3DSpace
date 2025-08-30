"use strict";

import { MVPTexROUnit } from './ROUnitModule.js';
import { ViewTransDesc } from './RenderCtx.js';

export class FBOUnit {

    constructor() {

        this.fbo = null;
        this.fboTex = null;
        this.glCtx = null;

        this.bindTexTimes = 0;
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

        this.bindTexTimes++;
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

export const TileParams = {
    defaultViewSize: 256,
    defaultViewLevel: 7,
    defaultViewFixedLevel: 9,
    defaultViewFixedSize: 512,
    defaultWorldLevel: 7,
    defaultWorldFixZoom: 0.9,
}

class TileRODesc {
    constructor(x, y, width, height, viewLevel) {

        // defaut value: world space coordinates
        this.x = x != undefined ? x : 0;
        this.y = y != undefined ? y : 0;

        this.width = width != undefined ? width : TileParams.defaultViewSize;
        this.height = height != undefined ? height : TileParams.defaultViewSize;
        this.viewLevel = viewLevel != undefined ? viewLevel : TileParams.defaultViewLevel;
        this.worldLevel = TileParams.defaultWorldLevel;

        this.dirty = true;

        this.wscRenderer = null;
        this.texture = null;
        this.fboUnit = null;

        this.viewTransDesc = new ViewTransDesc();
        this.viewTransDesc.setViewWorldBoundsXYWH(this.x, this.y, this.width, this.height);
        if (this.viewTransDesc.debugging) {
            this.viewTransDesc.clearColor.set([0.55, 0.95, 0.55, 1]);
        }
        this.hasDrawing = false;
    }
    destroy() {

        let gl = this.fboUnit.glCtx;
        if (this.texture) {
            gl.deleteTexture(this.texture);
        }
        if (this.viewTransDesc) {
            this.viewTransDesc.destroy();
        }
        this.wscRenderer = null;
        this.texture = null;
        this.fboUnit = null;
        this.viewTransDesc = null;
    }
    setWorldLevel(worldLevel) {

        if (worldLevel == undefined)
            return;

        if (this.worldLevel == worldLevel)
            return;

        this.worldLevel = worldLevel;
        this.dirty = true;

        let currSize = 2 << worldLevel;
        let worldZoom = currSize / TileParams.defaultViewSize;
        console.log("TileRODesc::setWorldLevel(), worldZoom: ", worldZoom, ", worldLevel: ", worldLevel);
        this.viewTransDesc.setViewWorldBoundsXYWH(this.x * worldZoom, this.y * worldZoom, this.width * worldZoom, this.height * worldZoom);

    }
    setViewLevel(viewLevel) {

        if (viewLevel == undefined)
            return;

        if (this.viewLevel == viewLevel)
            return;

        let gl = this.fboUnit.glCtx;
        if (this.texture) {
            console.log("TileRODesc::setViewLevel(), viewLevel: ", viewLevel, ", gl.deleteTexture() ...");
            gl.deleteTexture(this.texture);
            this.texture = null;
        }
        this.viewLevel = viewLevel;
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

        wscCtx.status.tileBuild();

        let gl = this.fboUnit.glCtx;

        let pw = this.width;
        let ph = this.height;

        let viewZoom = 1;

        if (this.viewLevel > TileParams.defaultViewLevel) {
            // 镜头拉近的放大过程
            viewZoom = TileParams.defaultViewFixedSize / pw;
            pw *= viewZoom;
            ph *= viewZoom;
        } else if (this.worldLevel > TileParams.defaultWorldLevel) {
            let currSize = 2 << this.worldLevel;
            viewZoom = TileParams.defaultViewSize / currSize;
        }


        let vtDesc = this.viewTransDesc;
        let worldBounds = vtDesc.viewWorldBounds;
        let vpx = worldBounds.x * viewZoom;
        let vpy = worldBounds.y * viewZoom;
        vtDesc.viewMat3.setTo(-vpx, -vpy, viewZoom, viewZoom);
        vtDesc.projMat3.ortho(pw, ph);

        console.log("TileRODesc::buildDraw(), viewLevel: ", this.viewLevel, ", worldLevel: ", this.worldLevel, ", viewZoom: ", viewZoom, ", size: ", pw, ", x: ", this.x, ",y: ", this.y);
        console.log("TileRODesc::buildDraw(), vtDesc: ", vtDesc);

        let fbo = this.fboUnit;
        fbo.bindFBO(gl);
        fbo.bindTexture(this.texture, pw, ph);
        this.texture = fbo.fboTex;

        let cvs = vtDesc.clearColor;
        gl.clearColor(cvs[0], cvs[1], cvs[2], cvs[3]);
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, pw, ph);
        
        vtDesc.status.drawTimes = 0;

        // 如果绘制的有实际内容，则这个tile有效，反之无效， 无效了之后这个tile资源就可以释放了
        wscRenderer.draw(vtDesc);

        this.hasDrawing = vtDesc.status.drawTimes > 0;
        // console.log("this.hasDrawing: ", this.hasDrawing, ", drawTimes: ", vtDesc.status.drawTimes);

        fbo.unbindFBO();
    }

    buildEnd() {
        this.wscRenderer.dirty = false;
        this.dirty = false;
    }
}
export class TileUnit {
    constructor(x, y, width, height, viewLevel) {

        this.roDesc = new TileRODesc(x, y, width, height, viewLevel);
        this.roUnit = new MVPTexROUnit();
    }

    setWorldLevel(worldLevel) {
        let roDesc = this.roDesc;
        roDesc.setWorldLevel(worldLevel);
        if (roDesc.dirty) {
            let currSize = 2 << roDesc.worldLevel;
            let worldZoom = currSize / TileParams.defaultViewSize;
            this.roUnit.setXY(roDesc.x * worldZoom, roDesc.y * worldZoom);
            this.roUnit.setScaleXY(roDesc.width * worldZoom, roDesc.height * worldZoom);
            console.log("TileRODesc::setWorldLevel(), worldZoom: ", worldZoom, ", worldLevel: ", worldLevel);
        }
    }

    setViewLevel(viewLevel) {
        let roDesc = this.roDesc;
        roDesc.setViewLevel(viewLevel);
    }
    destroy() {
        this.roDesc.destroy();
        if (this.roUnit) {
            this.roUnit.vertex = null;
            this.roUnit.shader = null;
            this.roUnit.textures = null;
            this.roUnit = null;
        }
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

    checkDrawing() {

        let desc = this.roDesc;
        let wscRenderer = desc.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;

        let hit = wscCtx.viewWorldBounds.intersects(desc.viewTransDesc.viewWorldBounds);
        return hit;
    }

    build() {

        if (!this.checkDrawing()) {
            // console.log("TileUnit::build(), false build() ...");
            return false;
        }

        let desc = this.roDesc;
        let tileTirty = desc.dirty;
        if (tileTirty) {
            desc.buildBegin();
            desc.buildDraw();
            desc.buildEnd();
            this.roUnit.setTextures([desc.texture]);
        }
        return tileTirty;
    }

    draw(ctx) {

        if (!this.checkDrawing()) {
            // console.log("TileUnit::draw(), false draw() ...");
            return;
        }

        let desc = this.roDesc;
        let wscRenderer = desc.wscRenderer;
        let moduleIns = wscRenderer.moduleIns;
        let wscCtx = moduleIns.viewTransDesc;
        let gl = desc.fboUnit.glCtx;

        let unit = this.roUnit;
        if (unit && unit.enabled && desc.hasDrawing) {

            ctx.status.tileDraw();
            // for debug
            if (wscCtx.debugging) {
                unit.colorData[0] = 0.9 + 0.2 * (ctx.status.tileDrawTimes % 6) / 6;
                unit.colorData[1] = 0.9 + 0.2 * (ctx.status.tileDrawTimes % 5) / 5;
            }
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

export class TileGrid {
    constructor(x, y, width, height, viewLevel) {


        this.unit = new TileUnit(x, y, width, height, viewLevel);
        this.viewLevel = this.unit.roDesc.viewLevel;
        this.worldLevel = TileParams.defaultWorldLevel;

        this.dirty = true;

        this.units = null;
    }

    // setWorldLevel(worldLevel) {
    //     this.roDesc.setWorldLevel(worldLevel);
    // }

    setWorldLevel(worldLevel) {

        if (worldLevel == undefined)
            return;

        if (this.worldLevel == worldLevel)
            return;

        this.worldLevel = worldLevel;
        this.dirty = true;
        this.unit.setWorldLevel(worldLevel);

        console.log("TileGrid::setWorldLevel(), worldLevel: ", worldLevel);
    }

    setViewLevel(viewLevel) {

        if (viewLevel == undefined)
            return;

        if (this.viewLevel == viewLevel)
            return;
        if (this.viewLevel >= TileParams.defaultViewFixedLevel && viewLevel < TileParams.defaultViewFixedLevel) {
            this.unit.enabled = true;
        }
        if (this.units) {
            for (let i = 0, ln = this.units.length; i < ln; ++i) {
                this.units[i].destroy();
            }
            this.units = null;
        }
        console.log("TileGrid::setViewLevel(), viewLevel: ", viewLevel);

        this.viewLevel = viewLevel;
        this.dirty = true;

        if (this.viewLevel < TileParams.defaultViewFixedLevel) {
            this.unit.setViewLevel(viewLevel);
        } else {

            let unit = this.unit;
            let roDesc = unit.roDesc;
            unit.enabled = false;

            let n = 2 << (this.viewLevel - TileParams.defaultViewFixedLevel);
            let dSize = roDesc.width / n;
            this.units = new Array(n * n);
            let k = 0;
            for (let i = 0; i < n; ++i) {
                let py = roDesc.y + i * dSize;
                for (let j = 0; j < n; ++j) {
                    let px = roDesc.x + j * dSize;
                    let pu = new TileUnit(px, py, dSize, dSize, viewLevel);
                    pu.initialize(roDesc.wscRenderer, roDesc.fboUnit, unit.roUnit);
                    this.units[k] = pu;
                    k++;
                }
            }
            console.log("TileGrid::setViewLevel(), this.units: ", this.units);
        }

    }
    initialize(wscRenderer, fboUnit, srcRoUnit) {
        this.unit.initialize(wscRenderer, fboUnit, srcRoUnit);
    }

    checkDrawing() {
        return this.unit.checkDrawing();
    }

    build() {

        if (!this.unit.checkDrawing()) {
            console.log("TileGrid::build(), false build() ...");
            return;
        }

        let dirty = this.dirty;

        let flag = false;
        if (this.viewLevel < TileParams.defaultViewFixedLevel) {
            this.unit.build();
        } else if (this.units) {
            for (let i = 0, ln = this.units.length; i < ln; ++i) {
                let fb = this.units[i].build();
                flag = flag || !fb;
            }
            this.dirty = flag;
        }

        return dirty;
    }

    draw(ctx) {
        if (!this.unit.checkDrawing()) {
            // console.log("TileGrid::draw(), false draw() ...");
            return;
        }
        if (this.viewLevel < TileParams.defaultViewFixedLevel) {
            this.unit.draw(ctx);
        } else if (this.units) {
            for (let i = 0, ln = this.units.length; i < ln; ++i) {
                this.units[i].draw(ctx);
            }
        }
    }
}