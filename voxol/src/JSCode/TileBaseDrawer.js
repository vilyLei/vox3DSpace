"use strict";
// import { SimpleCacheDrawer } from './SimpleCacheRenderer.js';
import { FBOUnit, TileUnit } from './FBOModule.js';

export class TileBaseDrawer {
    constructor() {

        this.glCtx = null;
        this.wscRenderer = null;
        this.fboIns = null;
        this.tile0 = null;
    }
    initialize(gl, wscRenderer) {

        this.glCtx = gl;
        this.wscRenderer = wscRenderer;
        this.fboIns = new FBOUnit();
        this.fboIns.initialize(gl);
        
        this.tile0 = new TileUnit(256, 256);
        this.tile0.initialize(this.fboIns, wscRenderer.mvpTexUnit);
    }

}