"use strict";

export function getVertsWithVEOSegN(n) {

    let x = 0, y = 0, w = 1, h = 1;

    if (n > 1) {
        let verts = new Float32Array(n * 12);
        for (let i = 0; i < n; ++i) {
            // transI 用于指定transform矩阵的序号
            let transI = i;
            verts.set([
                x, y, transI,
                x + w, y, transI,
                x + w, y + h, transI,
                x, y + h, transI], i * 12);
        }
        return verts;
    } else {
        let verts = new Float32Array([
            x, y,
            x + w, y,
            x + w, y + h,
            x, y + h]);
        return verts;
    }
}

export function getVertsWithUV() {

    let x = 0, y = 0, w = 1, h = 1;
    let verts = new Float32Array([
        x, y, 0, 0,
        x + w, y, 1, 0,
        x + w, y + h, 1, 1,
        x, y + h, 0, 1]);
    return verts;
}
export function getIndicesWithSegN(n) {


    const indices = new Uint16Array(n * 6);
    for (let i = 0; i < n; ++i) {
        let baseI = i * 4;
        indices.set([
            baseI, baseI + 1, baseI + 2,
            baseI + 2, baseI + 3, baseI], i * 6);
    }
    return indices;
}