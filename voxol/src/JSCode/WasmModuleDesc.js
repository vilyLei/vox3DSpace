"use strict";

// import { Mat33 } from './Mat33.js';
// import { Bounds2D } from './CGeomBase.js';
// import { ViewTransDesc } from './RenderCtx.js';

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

        this.elementDataIndex = bufIndex + 3;

        this.cmd = heapU32[bufIndex];
        this.bufSize = heapU32[bufIndex + 1];
        this.cmdsTotal = heapU32[bufIndex + 2];

        // console.log(`BatchElementDesc::update(), cmd: 0x${this.cmd.toString(16)}, cmdsTotal: ${this.cmdsTotal}`);

    }

}
