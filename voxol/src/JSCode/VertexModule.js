"use strict";
export class VtxUnit{

    constructor() {

        this.program = null;
        this.vao = null;
        this.locations = null;
        this.veo = null;
        this.indices = null;
    }

}
export class VertexBuilder {

    
    static createVAO(vtxUnit, gl, program, verts, compSizeArr, strideBytesArr, attributes) {


        if(vtxUnit == undefined || vtxUnit == null)
            vtxUnit = new VtxUnit();

        const vao = gl.createVertexArray();
        gl.bindVertexArray(vao);

        const vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
        gl.bufferData(gl.ARRAY_BUFFER, verts, gl.STATIC_DRAW);

        let locations = Array(compSizeArr.length);

        for(let i = 0; i < compSizeArr.length; ++i) {

            let compSize = compSizeArr[i];

            if (compSize == undefined || compSize < 1)
                compSize = 1;

            let strideBytes = strideBytesArr[i];
            if (strideBytes == undefined || strideBytes < 4)
                strideBytes = 4;

            const posLoc = gl.getAttribLocation(program, attributes[i]);
            gl.enableVertexAttribArray(posLoc);
            gl.vertexAttribPointer(posLoc, compSize, gl.FLOAT, false, strideBytes, 0);

            locations[i] = posLoc;
        }
        vtxUnit.program = program;
        vtxUnit.vao = vao;
        vtxUnit.locations = locations;

        return vtxUnit;
    }
    
    static createVEO(vtxUnit, gl, indices) {

        if(vtxUnit == undefined || vtxUnit == null)
            vtxUnit = new VtxUnit();

        let veo = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, veo);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

        vtxUnit.indices = indices;
        vtxUnit.veo = veo;
        return vtxUnit;
    }
};