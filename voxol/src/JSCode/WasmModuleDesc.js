"use strict";
export class ViewTransDesc {

    constructor() {

        this.dataIndex = -1;
        this.cmd = 0;
        this.bufSize = 0;

        this.heapU32 = null;
        this.heapF32 = null;

        this.projF32 = null;
        this.viewF32 = null;
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
        this.update();
    }

    update() {

        if (this.dataIndex < 0)
            return;

        let bufIndex = this.dataIndex;

        let heapU32 = this.heapU32;
        let heapF32 = this.heapF32;

        this.cmd = heapU32[bufIndex];
        this.bufSize = heapU32[bufIndex + 1];
        // console.log("ViewTransDesc::parse() cmd: ", this.cmd.toString(16), ", bufSize: ", this.bufSize);
        let projIndex = bufIndex + 2;
        let projmatvs = heapF32.subarray(projIndex, projIndex + 9);
        let viewIndex = bufIndex + 2 + 9;
        let viewmatvs = heapF32.subarray(viewIndex, viewIndex + 9);
        // console.log("ViewTransDesc::parse() projmatvs: ");
        // console.log(projmatvs);
        // console.log("ViewTransDesc::parse() viewmatvs: ");
        // console.log(viewmatvs);
        this.projF32 = projmatvs;
        this.viewF32 = viewmatvs;
    }
}

export class BatchElementDesc {

    constructor() {

        this.dataIndex = -1;
        this.elementDataIndex = -1;
        this.cmdsTotal = 0;

        this.cmd = 0;
        this.bufSize = 0;
        this.heapU32 = null;
        this.heapF32 = null;
    }
    getElementDataIndex() {
        return this.elementDataIndex;
    }
    parse(bufIndex) {

        if (bufIndex != undefined || bufIndex >= 0) {
            this.dataIndex = bufIndex;
        }
        this.update();
    }

    update() {

        if (this.dataIndex < 0)
            return;

        let bufIndex = this.dataIndex;
        let heapU32 = this.heapU32;
        // let heapF32 = this.heapF32;

        this.elementDataIndex = bufIndex + 3;

        this.cmd = heapU32[bufIndex];
        this.bufSize = heapU32[bufIndex + 1];
        this.cmdsTotal = heapU32[bufIndex + 2];

        // console.log(`BatchElementDesc::update(), cmd: 0x${this.cmd.toString(16)}, cmdsTotal: ${this.cmdsTotal}`);

    }

}
