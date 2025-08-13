"use strict";

// 列主序3x3矩阵
export class Mat33 {

    constructor(fs32Arr9) {
        if (fs32Arr9 == undefined || fs32Arr9.length == undefined || fs32Arr9.length != 9) {
            this.data = new Float32Array(
                [1, 0, 0,
                    0, 1, 0,
                    0, 0, 1]
            );
        } else {
            this.data = new Float32Array(fs32Arr9);
        }
    }

    setTo(tx, ty, sx = 1, sy = 1, rotRadians = 0) {

        let c = Math.cos(rotRadians);
        let s = Math.sin(rotRadians);

        this.data.set([c * sx, s * sx, 0,
        -s * sy, c * sy, 0,
            tx, ty, 1]);
    }

    ortho(width, height) {

        this.data.set([2 / width, 0, 0,
            0, -2 / height, 0,
        -1, 1, 1]);
    }

    // 列主序矩阵的前乘(左乘), 性能更好
    prepend(lhs) {

        let sfs = this.data;
        let lfs = lhs.data;

        for (let i = 0; i < 3; ++i) {
            let rc0 = sfs[i * 3 + 0];
            let rc1 = sfs[i * 3 + 1];
            let rc2 = sfs[i * 3 + 2];
            sfs[i * 3 + 0] = rc0 * lfs[0] + rc1 * lfs[3] + rc2 * lfs[6];
            sfs[i * 3 + 1] = rc0 * lfs[1] + rc1 * lfs[4] + rc2 * lfs[7];
            sfs[i * 3 + 2] = rc0 * lfs[2] + rc1 * lfs[5] + rc2 * lfs[8];
        }
    }
    // 列主序矩阵的后乘(右乘), 性能略弱
    append(rhs) {
        let sfs = this.data;
        let rfs = rhs.data;
        let result = new Float32Array(9);

        for (let i = 0; i < 3; ++i) {
            let rc0 = rfs[i * 3 + 0];
            let rc1 = rfs[i * 3 + 1];
            let rc2 = rfs[i * 3 + 2];

            result[i * 3 + 0] = sfs[0] * rc0 + sfs[3] * rc1 + sfs[6] * rc2;
            result[i * 3 + 1] = sfs[1] * rc0 + sfs[4] * rc1 + sfs[7] * rc2;
            result[i * 3 + 2] = sfs[2] * rc0 + sfs[5] * rc1 + sfs[8] * rc2;
        }
        sfs.set(result);
    }

    print() {
        let data = this.data;
        console.log("{\n");
        for (let i = 0; i < 3; ++i) {
            console.log(
                data[i * 3 + 0].toFixed(5),
                data[i * 3 + 1].toFixed(5),
                data[i * 3 + 2].toFixed(5));
        }
        console.log("}\n");
    }
}