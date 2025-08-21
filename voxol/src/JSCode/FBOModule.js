"use strict";


export class FBOBaseIns {
    constructor() {
        this.fbo = null;
        this.fboTex = null;
    }

    initialize(gl, fbo) {
        this.fbo = fbo == undefined ? gl.createFramebuffer() : fbo;
    }

    bindTexture(gl, fboTex, width, height) {

        this.fboTex = fboTex == undefined ? gl.createTexture() : fboTex;
        gl.bindTexture(gl.TEXTURE_2D, this.fboTex);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.fboTex, 0);
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }

    bindFBO(gl) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.fbo);
    }

    unbindFBO(gl) {
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
}