"use strict";
import { Mat33 } from './Mat33.js';
import { Bounds2D } from './CGeomBase.js';

export class RenderStatus {
    constructor() {
        this.tileBuildTimes = 0;
        this.tileDrawTimes = 0;
    }
    reset() {
        this.tileBuildTimes = 0;
        this.tileDrawTimes = 0;
    }
    tileBuild() {
        this.tileBuildTimes ++;
    }
    tileDraw() {
        this.tileDrawTimes ++;
    }
    print() {        
        console.log(`RenderStatus( tileBuildTimes = ${this.tileBuildTimes}, tileDrawTimes = ${this.tileDrawTimes})`);
    }
}
export class ViewTransDesc {

    constructor() {

        this.dataIndex = -1;
        this.cmd = 0;
        this.bufSize = 0;

        this.heapU32 = null;
        this.heapF32 = null;

        this.projMat3 = new Mat33();
        this.viewMat3 = new Mat33();
        this.viewInvMat3 = new Mat33();

        this.projF32 = this.projMat3.data;
        this.viewF32 = this.viewMat3.data;

        this.viewBounds = new Bounds2D(0, 0, 512, 512);
        // view bounds in the world space
        this.viewWorldBounds = new Bounds2D(0, 0, 512, 512);

        this.status = new RenderStatus();
    }
    destroy() {

        this.heapU32 = null;
        this.heapF32 = null;

        this.projMat3 = null;
        this.viewMat3 = null;
        this.viewInvMat3 = null;

        this.projF32 = null;
        this.viewF32 = null;

        this.viewBounds = null;
        // view bounds in the world space
        this.viewWorldBounds = null;
    }

    getZoom() {

        if (this.viewF32)
            return this.viewF32[0];

        return 1;
    }

    parse(bufIndex) {

        if (bufIndex != undefined || bufIndex >= 0) {
            this.dataIndex = bufIndex;
        }
        this.updateData();
    }

    update() {

        this.updateData();

        this.viewMat3.inverseTo(this.viewInvMat3);
        this.viewBounds.mapWithMat33To(this.viewInvMat3, this.viewWorldBounds);

        // console.log("this.viewBounds: ", this.viewBounds);
        // console.log("this.viewWorldBounds: ", this.viewWorldBounds);
    }
    updateData() {

        if (this.dataIndex < 0)
            return;

        let bufIndex = this.dataIndex;

        let heapU32 = this.heapU32;
        let heapF32 = this.heapF32;

        this.cmd = heapU32[bufIndex];
        this.bufSize = heapU32[bufIndex + 1];
        // console.log("ViewTransDesc::updateData() cmd: ", this.cmd.toString(16), ", bufSize: ", this.bufSize);
        let projIndex = bufIndex + 2;
        let projmatvs = heapF32.subarray(projIndex, projIndex + 9);
        let viewIndex = bufIndex + 2 + 9;
        let viewmatvs = heapF32.subarray(viewIndex, viewIndex + 9);
        // console.log("ViewTransDesc::updateData() projmatvs: ");
        // console.log(projmatvs);
        // console.log("ViewTransDesc::updateData() viewmatvs: ");
        // console.log(viewmatvs);
        this.projF32.set(projmatvs);
        this.viewF32.set(viewmatvs);
    }
}
