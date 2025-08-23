"use strict";

export function printWith9Number(numArr, index) {

    let f32Str = "";
    for (let i = 0; i < 9; i++) {
        f32Str += numArr[index + i];
        f32Str += ((i + 1) % 3) == 0 ? ",\n" : (i < 8 ? "," : "");
    }
    console.log("array9Numbers:");
    console.log(f32Str);
}

class BatchDrawer {
    constructor(){

    }
}

export class SimpleCacheDrawer {

    constructor() {

        this.moduleIns = null;
        this.glCtx = null;
        this.ctxWidth = 512;
        this.ctxHeight = 512;

        this.roScene = null;

        this.dirty = true;
        this.drewTotal = 0;

    }

    initialize(moduleIns, gl, vw, vh) {

        this.moduleIns = moduleIns;
        this.glCtx = gl;
        this.ctxWidth = vw;
        this.ctxHeight = vh;

        console.log("SimpleCacheDrawer::initialize() ...\n");
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

        let gl = this.glCtx;
        let moduleIns = this.moduleIns;
        ctx = ctx == undefined ? moduleIns.viewTransDesc : ctx;

        let scene = this.roScene;

        // let unit = scene.screenBgColorUnit;
        // if (unit.enabled) {
        //     unit.bind(gl, ctx);
        //     unit.draw(gl, ctx);
        // }
        this.drawBatch(ctx);
        
        let units = scene.testUnits;
        for (let i = 0; i < units.length; ++i) {
            const unit = units[i];
            if (unit && unit.enabled) {
                unit.bind(gl, ctx);
                unit.draw(gl, ctx);
            }
        }
    }
    drawBatch(ctx) {

        let scene = this.roScene;

        let moduleIns = this.moduleIns;
        let gl = this.glCtx;
        // ctx = ctx == undefined ? moduleIns.viewTransDesc : ctx;

        let matTot = scene.batchTotal;
        let drawIndex = 0;
        let drewTot = 0;

        let batchEle = moduleIns.batchEleDesc;
        let cmdIndex = batchEle.getElementDataIndex();
        let dataU32 = batchEle.heapU32;
        let dataF32 = batchEle.heapF32;

        let unit = scene.batchUnit;

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
            console.log("batch drew total: ", this.drewTotal);
        }

    }
    runEnd() {
        this.dirty = false;
    }
}