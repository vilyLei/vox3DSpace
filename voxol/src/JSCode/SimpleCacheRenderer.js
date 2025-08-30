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
    constructor() {

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

        
        let moduleIns = this.moduleIns;
        let vtDesc = moduleIns.viewTransDesc;

        // gl.clearColor(0.95, 0.95, 0.95, 1);
        let cvs = vtDesc.clearColor;
        gl.clearColor(cvs[0], cvs[1], cvs[2], cvs[3]);

        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.viewport(0, 0, vw, vh);

    }

    draw(ctx) {

        let gl = this.glCtx;
        let moduleIns = this.moduleIns;
        let vDesc = moduleIns.viewTransDesc;
        ctx = ctx == undefined ? vDesc : ctx;

        let scene = this.roScene;
        let drcUnits = scene.drcUnits;

        let vwBounds = vDesc.viewWorldBounds;
        // let bgUnit =  scene.bgUnit;
        // if(bgUnit && bgUnit.enabled) {
        //     let dis = 5.0 / moduleIns.getZoom();
        //     bgUnit.setXY(vwBounds.x + dis, vwBounds.y + dis);
        //     bgUnit.setScaleXY(vwBounds.width - 2 * dis, vwBounds.height - 2 * dis);
        //     bgUnit.bind(gl, ctx);
        //     bgUnit.draw(gl, ctx);
        // }

        // let unit = scene.screenBgColorUnit;
        // if (unit.enabled) {
        //     unit.bind(gl, ctx);
        //     unit.draw(gl, ctx);
        // }

        let batchEle = moduleIns.batchEleDesc;
        // let cmdIndex = batchEle.getElementDataIndex();
        // this.drawBatch(ctx, cmdIndex);

        let heapU32 = moduleIns.heapU32;
        let heapF32 = moduleIns.heapF32;

        let cmdIndex = moduleIns.renderingDataIndex;
        let loop = true;
        for (; loop;) {
            let cmd = heapU32[cmdIndex];
            switch (cmd) {
                case 22:
                    cmdIndex++;
                    batchEle.parse(cmdIndex);
                    cmdIndex = batchEle.getElementDataIndex();
                    cmdIndex = this.drawBatch(ctx, cmdIndex, batchEle.cmdsTotal);
                    // loop = false;
                    break;
                case 0x32:
                    let descSize = heapU32[cmdIndex + 1];
                    let mroid = heapU32[cmdIndex + 6];
                    let rounit = drcUnits[mroid];
                    rounit.parse(cmdIndex, heapU32, heapF32);
                    if(rounit.enabled) {
                        rounit.bind(gl, ctx);
                        rounit.draw(gl, ctx);
                    }
                    cmdIndex += descSize;
                    break;
                    
                case 0x0:
                    if(heapU32[cmdIndex + 1] == 0x0) {
                        // console.log("SimpleCacheDrawer::initialize() exec cmds to buf tail !!!\n");
                        loop = false;
                    }
                    break;
                default:
                    break;

            }
        }

        // let units = scene.testUnits;
        // for (let i = 0; i < units.length; ++i) {
        //     const unit = units[i];
        //     if (unit && unit.enabled) {
        //         unit.bind(gl, ctx);
        //         unit.draw(gl, ctx);
        //     }
        // }
    }
    drawBatch(ctx, cmdIndex, total) {

        if (total == undefined)
            total = 0xffffff;

        let scene = this.roScene;

        let moduleIns = this.moduleIns;
        let gl = this.glCtx;

        let tot = 0;
        let matTot = scene.batchTotal;
        let drawIndex = 0;
        let drewTot = 0;

        let batchEle = moduleIns.batchEleDesc;
        let dataU32 = batchEle.heapU32;
        let dataF32 = batchEle.heapF32;

        let unit = scene.batchUnit;

        unit.bind(gl, ctx);

        for (; ;) {
            if (drewTot >= 180000) {
                break;
            }
            if (tot >= total) {
                // console.log(`drawing batch cmds all tot=${tot}, total=${total} !!!`);
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
                    {
                        tot++;
                        unit.parse(drawIndex, cmdIndex, dataU32, dataF32);
                    }
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
            // console.log("batch drew total: ", this.drewTotal);
        }
        return cmdIndex;
    }
    runEnd() {
        this.dirty = false;
    }
}