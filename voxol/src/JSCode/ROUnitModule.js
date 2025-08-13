
import { ShaderUnit, ShaderBuilder } from './ShaderModule.js';
import { VtxUnit, VertexBuilder } from './VertexModule.js';

export class ROUnit {
    constructor() {
        this.shader = null
        this.vertex = null
    }
    initialize() {
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        this.transData = new Float32Array([1, 0, 0,
            0, 1, 0,
            0, 0, 1]);
        this.colorData = new Float32Array([0.8, 0.8, 0.8, 1]);
    }
    bind(gl) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl) {
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
    initialize() {
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        this.objMatData = new Float32Array(
            [50, 0, 0,
                0, 50, 0,
                0, 0, 1]);
        this.viewMatData = null;
        this.projMatData = null;
        this.colorData = new Float32Array([0, 0.8, 0.0, 1]);
    }

    bind(gl) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }

    draw(gl) {

        let uniforms = this.shader.uniforms;

        gl.uniformMatrix3fv(uniforms[0].location, false, this.objMatData, 0, 9);
        gl.uniformMatrix3fv(uniforms[1].location, false, this.viewMatData, 0, 9);
        gl.uniformMatrix3fv(uniforms[2].location, false, this.projMatData, 0, 9);

        gl.uniform4fv(uniforms[3].location, this.colorData, 0, 4);
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
    destroy() { }
}

export class BatchROUnit extends ROUnit {
    constructor(tot) {
        super();
        this.matTotal = tot;
    }
    initialize() {
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
        let tot = this.matTotal;
        this.transData = new Float32Array(tot * 9);
        this.colorData = new Float32Array(tot * 4);
    }
    bind(gl) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl) {
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