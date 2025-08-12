
import { ShaderUnit, ShaderBuilder } from './ShaderModule.js';
import { VtxUnit, VertexBuilder } from './VertexModule.js';
export class ROUnit {
    constructor() {
        this.shader = new ShaderUnit();
        this.vertex = new VtxUnit();
    }
    bind(gl) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl) {
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
}
export class BatchROUnit extends ROUnit {
    constructor() {
        super();
    }
    bind(gl) {
        gl.useProgram(this.shader.program);
        gl.bindVertexArray(this.vertex.vao);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.vertex.veo);
    }
    draw(gl) {
        // let uniforms = this.shader.uniforms;
        // gl.uniformMatrix3fv(uniforms[0].location, false, transData, 0, matTot * 9);
        // gl.uniform4fv(uniforms[1].location, colorData, 0, matTot * 4);
        gl.drawElements(gl.TRIANGLES, this.vertex.indices.length, gl.UNSIGNED_SHORT, 0);
    }
}