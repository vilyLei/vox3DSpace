"use strict";

import { MVPTexROUnit, BatchROUnit, MVPROUnit, ROUnit } from './ROUnitModule.js';
export class TileUnit {
    constructor(width, height){
        
        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

        this.texture = null;
        this.roUnit = new MVPTexROUnit();
        this.fboUnit = null;
    }
    initialize(fboUnit, srcRoUnit) {
        this.fboUnit = fboUnit;
        this.roUnit.shader = srcRoUnit.shader;
        this.roUnit.vertex = srcRoUnit.vertex;
        this.roUnit.initialize({ scaleX: this.width, scaleY: this.height, texturesNumber: 1 });
    }
    render() {
    }
    update() {
        this.roUnit.setScaleXY(this.width, this.height);
    }

}
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

    bindFBO(gl) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
    }

    bindTexture(gl, fboTex, width, height) {

        this.fboTex = fboTex == undefined ? gl.createTexture() : fboTex;

        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);

        if(fboTex == undefined) {
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        }

        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);
    }

    unbindFBO(gl) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
}