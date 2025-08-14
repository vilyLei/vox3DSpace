"use strict";

import { ModuleInstance } from './WasmModuleIns.js';

function normlizeViewSize() {

    let pw = window.innerWidth;
    let ph = window.innerHeight;
    const dpr = window.devicePixelRatio || 1;
    pw = Math.round(pw * dpr);
    ph = Math.round(ph * dpr);
    return [pw, ph];
}

function calcMouseXY(event) {

    const clientX = event.clientX;
    const clientY = event.clientY;
    const rect = canvas.getBoundingClientRect();
    const canvasX = rect.left;
    const canvasY = rect.top;

    let x = clientX - canvasX;
    let y = clientY - canvasY;
    const dpr = window.devicePixelRatio || 1;
    x *= dpr;
    y *= dpr;
    return { x: x, y: y };
}


class CtxViewParam {

    constructor() {

        this.width = 1;
        this.height = 1;
        this.dpr = 1;
        this.originWidth = 1;
        this.originHeight = 1;
        this.originDpr = 1;

    }
}

export class ModuleWrapper {

    constructor() {
        this.enabled = false;
        this.canvas = null;
        this.gl = null;
        this.ins = new ModuleInstance();
        this.ctxViewParam = new CtxViewParam();
    }
    isEnabled() {
        return this.enabled;
    }
    initialize(module, canvas, gl) {

        // console.log("ModuleWrapper::initialize() ...");

        let ins = this.ins;

        ins.initialize(module);

        this.canvas = canvas;
        this.gl = gl;

        let preHeapSize = 0;
        setInterval(() => {
            const size = ins.getHeapSize();
            if (preHeapSize != size) {
                preHeapSize = size;
                console.log("WASM 堆内存大小:", (size / 1024 / 1024).toFixed(2), "MB");
            }
        }, 1000);
        window.addEventListener('resize', () => {
            this.resizeCanvas();
        });

        this.resizeCanvas();
        this.startup();
        ins.initHeapParse();
        this.initEvent();
        let batchEleDesc = this.ins.batchEleDesc;
        this.enabled = batchEleDesc.getElementDataIndex() > 1;
    }
    initEvent() {
        let canvas = this.canvas;
        let ins = this.ins;

        canvas.addEventListener('mousedown', event => {
            let pos = calcMouseXY(event);
            // console.log('mousedown(' + pos.x + ', ' + pos.y + ')');
            // console.log("event.button: ", event.button);

            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 1, 0);
        });
        canvas.addEventListener('mouseup', event => {
            let pos = calcMouseXY(event);
            // console.log('mouseup(' + pos.x + ', ' + pos.y + ')');
            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 2, 0);
        });
        window.addEventListener('mouseup', event => {
            let pos = calcMouseXY(event);
            // console.log('mouseup(' + pos.x + ', ' + pos.y + ')');
            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 2, 0);
        });
        canvas.addEventListener('mousemove', event => {
            let pos = calcMouseXY(event);
            // console.log('mousemove(' + pos.x + ', ' + pos.y + ')');
            // console.log("mousemove, event.button: ", event.button);
            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 3, 0);
        });
        canvas.addEventListener('mousewheel', event => {
            let pos = calcMouseXY(event);
            // console.log('mousewheel(' + pos.x + ', ' + pos.y + ')');
            // console.log("mousewheel, event.deltaY: ", event.deltaY);
            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 4, event.deltaY);
        });
        canvas.addEventListener('click', event => {
            let pos = calcMouseXY(event);
            console.log('click(' + pos.x + ', ' + pos.y + ')');
            ins.setMouseParamsFunc(pos.x, pos.y, event.button * 10 + 5);
        });
    }
    startup() {
        this.ins.startupFunc();
    }
    run() {
        return this.ins.runExecFunc();
    }

    resizeCanvas() {

        let ctxViewParam = this.ctxViewParam;

        console.log("window.devicePixelRatio: ", window.devicePixelRatio);
        const dpr = window.devicePixelRatio || 1;

        const [w, h] = normlizeViewSize();
        // console.log(`resizeCanvas, w=${w}, h=${h}`);
        ctxViewParam.width = w;
        ctxViewParam.height = h;
        ctxViewParam.dpr = dpr;
    }

    updateCtxViewParam(callback) {

        let ctxViewParam = this.ctxViewParam;
        let pw = ctxViewParam.width;
        let ph = ctxViewParam.height;
        let dpr = ctxViewParam.dpr;

        if (ctxViewParam.originWidth != pw || ctxViewParam.originHeight != ph || ctxViewParam.originDpr != dpr) {
            console.log(`ModuleWrapper::updateCtxViewParam(w=${pw},h=${ph},dpr=${dpr})`);
            ctxViewParam.originWidth = pw;
            ctxViewParam.originHeight = ph;
            ctxViewParam.originDpr = dpr;

            let canvas = this.canvas;
            canvas.width = pw;
            canvas.height = ph;
            canvas.style.width = `${pw / dpr}px`;
            canvas.style.height = `${ph / dpr}px`;

            this.ins.setGPUCtxSizeFunc(pw, ph);
            if (callback) {
                callback(pw, ph);
            }
        }
    }
}