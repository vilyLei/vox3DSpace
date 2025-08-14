"use strict";
export class ShaderUniformDesc {
    // name: uProjMat
    // type: mat4
    constructor(name, type) {
        this.name = name;
        this.type = type;
    }
}
export class ShaderUniform {

    constructor() {
        this.location = null;
        this.desc = null;
    }

}

export class ShaderUnit {

    constructor() {
        this.program = null;
        this.uniforms = null;
    }

}
export class ShaderBuilder {
    static createShader(gl, type, source) {
        console.log(source);
        const shader = gl.createShader(type);
        gl.shaderSource(shader, source);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(shader));
            gl.deleteShader(shader);
            return null;
        }
        return shader;
    }

    static createProgram(gl, vs, fs) {
        const program = gl.createProgram();
        gl.attachShader(program, vs);
        gl.attachShader(program, fs);
        gl.linkProgram(program);
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            console.error(gl.getProgramInfoLog(program));
            return null;
        }
        return program;
    }

    static createShaderProgram(gl, vsSource, fsSource) {
        console.log("ShaderBuilder::createShaderProgram() ...");
        const vs = ShaderBuilder.createShader(gl, gl.VERTEX_SHADER, vsSource);
        const fs = ShaderBuilder.createShader(gl, gl.FRAGMENT_SHADER, fsSource);
        const program = ShaderBuilder.createProgram(gl, vs, fs);

        return program;
    }
    static createShaderUnit(shaderUnit, gl, vsSource, fsSource, shaderUniformDescs) {

        let unit = shaderUnit ? shaderUnit : new ShaderUnit();
        unit.program = ShaderBuilder.createShaderProgram(gl, vsSource, fsSource);
        let tot = 0;
        let flag = shaderUniformDescs != undefined && shaderUniformDescs != null && shaderUniformDescs.length != undefined;
        if (flag && shaderUniformDescs.length > 0) {
            tot = shaderUniformDescs.length;
            unit.uniforms = new Array(tot);
        }
        for (let i = 0; i < tot; ++i) {
            unit.uniforms[i] = new ShaderUniform();
            unit.uniforms[i].location = gl.getUniformLocation(unit.program, shaderUniformDescs[i].name);
            unit.uniforms[i].desc = shaderUniformDescs[i];
        }
        return unit;
    }
}