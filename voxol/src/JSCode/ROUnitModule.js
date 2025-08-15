"use strict";

import { ShaderUnit, ShaderBuilder } from './ShaderModule.js';
import { VtxUnit, VertexBuilder } from './VertexModule.js';

export class ROUnit {
    constructor() {
        this.shader = null;
        this.vertex = null;
        this.enabled = false;
    }
    initialize(params) {
        this.enabled = true;
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        this.transData = new Float32Array([1, 0, 0,
            0, 1, 0,
            0, 0, 1]);
        this.colorData = new Float32Array([0.8, 0.8, 0.8, 1]);
    }
    setXY(x, y) {
        x = x != undefined ? x : 0;
        y = y != undefined ? y : 0;
        this.objMatData[6] = x;
        this.objMatData[7] = y;
    }
    setScaleXY(sx, sy) {
        sx = sx != undefined ? sx : 0;
        sy = sy != undefined ? sy : 0;
        this.objMatData[0] = sx;
        this.objMatData[4] = sy;
    }
    bind(gl, ctx) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl, ctx) {
        let uniforms = this.shader.uniforms;
        gl.uniformMatrix3fv(uniforms[0].location, false, this.transData, 0, 9);
        gl.uniform4fv(uniforms[1].location, this.colorData, 0, 4);
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
    destroy() { }
}

export class MVPROUnit extends ROUnit {
    constructor() {
        super();
    }
    initialize(params) {
        let sx = 1;
        let sy = 1;
        let px = 0;
        let py = 0;
        if (params !== undefined) {
            sx = params.scaleX !== undefined ? params.scaleX : 1;
            sy = params.scaleY !== undefined ? params.scaleY : 1;
            px = params.x !== undefined ? params.x : 0;
            py = params.y !== undefined ? params.y : 0;
        }
        this.enabled = true;
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        this.objMatData = new Float32Array(
            [sx, 0, 0,
                0, sy, 0,
                px, py, 1]);
        this.colorData = new Float32Array([0, 0.8, 0.0, 1]);
    }
    clone() {

        let unit = new MVPROUnit();
        unit.enabled = this.enabled;
        unit.shader = this.shader;
        unit.vertex = this.vertex;
        unit.objMatData = this.objMatData.slice();
        unit.colorData = this.colorData.slice();
        return unit;
    }
    bind(gl, ctx) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }

    draw(gl, ctx) {

        let uniforms = this.shader.uniforms;

        gl.uniformMatrix3fv(uniforms[0].location, false, this.objMatData, 0, 9);
        gl.uniformMatrix3fv(uniforms[1].location, false, ctx.viewF32, 0, 9);
        gl.uniformMatrix3fv(uniforms[2].location, false, ctx.projF32, 0, 9);

        gl.uniform4fv(uniforms[3].location, this.colorData, 0, 4);
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
    destroy() { }
}


export class MVPTexROUnit extends ROUnit {
    constructor() {
        super();
        this.textures = null;
    }
    initialize(params) {

        let sx = 1;
        let sy = 1;
        let px = 0;
        let py = 0;
        let texNum = 0;
        if (params !== undefined) {
            sx = params.scaleX !== undefined ? params.scaleX : 1;
            sy = params.scaleY !== undefined ? params.scaleY : 1;
            px = params.x !== undefined ? params.x : 0;
            py = params.y !== undefined ? params.y : 0;
            texNum = params.texturesNumber !== undefined ? params.texturesNumber : 0;
        }

        this.enabled = true;
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        this.objMatData = new Float32Array(
            [sx, 0, 0,
                0, sy, 0,
                px, py, 1]);
        this.colorData = new Float32Array([0, 0.8, 0.0, 1]);

        if (texNum > 0) {
            this.textures = new Array(texNum).fill(null);
        }
    }

    clone() {

        let unit = new MVPROUnit();
        unit.enabled = this.enabled;
        unit.shader = this.shader;
        unit.vertex = new VtxUnit();
        unit.objMatData = this.objMatData.slice();
        unit.colorData = this.colorData.slice();
        if (this.textures != null) {
            this.textures = this.textures.slice();
        }
        return unit;
    }
    checkTextures() {

        if (this.textures == null)
            return;

        this.enabled = false;
        for (let i = 0; i < this.textures.length; ++i) {
            if (this.textures[i] == null)
                return;
        }
        this.enabled = true;
    }
    setTextureAt(tex, index) {
        if (tex == undefined || this.textures == null || index < 0 || index >= this.textures.length)
            return;
        this.textures[index] = tex;
        this.checkTextures();
    }
    setTextures(textures) {

        if (textures == undefined)
            return;
        this.textures = textures;
        this.checkTextures();
    }

    bind(gl, ctx) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }

    draw(gl, ctx) {

        let uniforms = this.shader.uniforms;

        gl.uniformMatrix3fv(uniforms[0].location, false, this.objMatData, 0, 9);
        gl.uniformMatrix3fv(uniforms[1].location, false, ctx.viewF32, 0, 9);
        gl.uniformMatrix3fv(uniforms[2].location, false, ctx.projF32, 0, 9);

        gl.uniform4fv(uniforms[3].location, this.colorData, 0, 4);

        let textures = this.textures;
        if (textures != null && textures.length > 0) {
            let tus = this.shader.texUniforms;
            for (let i = 0; i < textures.length; ++i) {
                gl.activeTexture(gl.TEXTURE0 + i);
                gl.bindTexture(gl.TEXTURE_2D, textures[i]);
                gl.uniform1i(tus[i].location, i);
            }
        }

        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
    destroy() { }
}

export class BatchROUnit extends ROUnit {
    constructor(tot) {
        super();
        this.matTotal = tot;
    }
    initialize(params) {
        this.enabled = true;
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        let tot = this.matTotal;
        this.transData = new Float32Array(tot * 9);
        this.colorData = new Float32Array(tot * 4);
    }
    bind(gl, ctx) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl, ctx) {
        let tot = this.matTotal;
        let uniforms = this.shader.uniforms;
        gl.uniformMatrix3fv(uniforms[0].location, false, this.transData, 0, tot * 9);
        gl.uniform4fv(uniforms[1].location, this.colorData, 0, tot * 4);
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
    parse(drawIndex, cmdIndex, dataU32, dataF32) {

        let f32BoundsIndex = cmdIndex + 2;
        let boundsvs = dataF32.subarray(f32BoundsIndex, f32BoundsIndex + 4);

        let colorU32 = dataU32[cmdIndex + 6];
        let a = ((colorU32 >> 24) & 0xff) / 255.0;
        let r = ((colorU32 >> 16) & 0xff) / 255.0;
        let g = ((colorU32 >> 8) & 0xff) / 255.0;
        let b = (colorU32 & 0xff) / 255.0;
        this.colorData.set([r, g, b, a], drawIndex * 4);

        let f32Index = cmdIndex + 7;
        let matvs = dataF32.subarray(f32Index, f32Index + 9);
        this.transData.set(matvs, drawIndex * 9);
    }
}