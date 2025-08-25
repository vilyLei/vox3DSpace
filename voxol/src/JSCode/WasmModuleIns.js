"use strict";

import { BatchElementDesc, ViewTransDesc } from './WasmModuleDesc.js';

export class ModuleInstance {

    constructor() {

        this.module = null;
        this.heapU8 = null;
        this.heapU32 = null;
        this.heapF32 = null;

        this.viewTransDesc = new ViewTransDesc();
        this.batchEleDesc = new BatchElementDesc();

        this.renderingDataIndex = 0;
        this.version = 0;
    }

    initialize(module) {

        // console.log("ModuleInstance::initialize() ...");

        this.module = module;

        this.getHeapSize = module.cwrap("get_heap_size_bytes", "number", []);
        this.startupFunc = module.cwrap("startup");
        this.setGPUCtxSizeFunc = module.cwrap("setGPUCtxSize", null, ["number", "number"]);
        this.setMouseParamsFunc = module.cwrap("setMouseParams", null, ["number", "number", "number", "number"]);

        this.runExecFunc = module.cwrap("run", "number");
        this.getCmdsTotalExec = module.cwrap("getCmdsTotal", "number");
        this.getCmdsExec = module.cwrap("getCmds", "number");
        this.getRenderCmdBufferExec = module.cwrap("getRenderCmdBuffer", "number");
    }
    updateHeapInfo() {

        let heapU8 = this.module.HEAPU8;
        let cmdU32Arr = new Uint32Array(heapU8.buffer);
        let cmdU8Ptr = this.getCmdsExec();
        let cmdBufU8Ptr = this.getRenderCmdBufferExec();
        cmdU32Arr = new Uint32Array(heapU8.buffer);
        cmdU32Arr = cmdU32Arr.subarray(cmdU8Ptr / 4);

        let cmdBufF32Arr = new Float32Array(heapU8.buffer);
        cmdBufF32Arr = cmdBufF32Arr.subarray(cmdBufU8Ptr / 4);

        let cmdBufU32Arr = new Uint32Array(heapU8.buffer);
        cmdBufU32Arr = cmdBufU32Arr.subarray(cmdBufU8Ptr / 4);

        this.heapU8 = heapU8;
        this.heapU32 = cmdBufU32Arr;
        this.heapF32 = cmdBufF32Arr;

        this.viewTransDesc.heapU32 = this.heapU32;
        this.viewTransDesc.heapF32 = this.heapF32;
        this.batchEleDesc.heapU32 = this.heapU32;
        this.batchEleDesc.heapF32 = this.heapF32;
    }

    initHeapParse() {

        this.runExecFunc();
        this.updateHeapHead();
    }
    updateHeapHead() {

        this.updateHeapInfo();

        let cmdBufU32Arr = this.heapU32;

        let bufIndex = 2;
        this.version = cmdBufU32Arr[bufIndex];
        // console.log("version: ", this.version);
        bufIndex++;
        let trunkCmd = cmdBufU32Arr[bufIndex];
        // console.log("trunkCmd: ", trunkCmd);
        bufIndex++;

        if (trunkCmd == 20) {

            this.viewTransDesc.parse(bufIndex);
            bufIndex += this.viewTransDesc.bufSize;

            trunkCmd = cmdBufU32Arr[bufIndex];
            // console.log("trunkCmd: ", trunkCmd);
            bufIndex++;
        }
        if (trunkCmd == 21) {
            console.log("rendering begin trunkCmd: ", trunkCmd);
            this.renderingDataIndex = bufIndex;
            trunkCmd = cmdBufU32Arr[bufIndex];
            bufIndex++;
        }

        if (trunkCmd == 22) {
            
            this.batchEleDesc.parse(bufIndex);
            bufIndex += this.batchEleDesc.bufSize;
        }

        // let cmdsTotal = this.getCmdsTotalExec();
        // this.rcmsTotal = cmdsTotal;
        // console.log("B cmdsTotal: ", cmdsTotal);
        // console.log("bufIndex: ", bufIndex);
        // this.heapDataIndex = bufIndex;
    }
}