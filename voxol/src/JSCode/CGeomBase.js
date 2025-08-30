"use strict";

export class Bounds2D {

    constructor(x, y, width, height) {

        this.setXYWH(x, y, width, height);
    }

    copyFrom(src) {
        this.setXYWH(src.x, src.y, src.width, src.height);
    }

    setXY(x, y) {

        this.x = x != undefined ? x : 0;
        this.y = y != undefined ? y : 0;
        this.updateLTRB();
    }

    setWH(width, height) {
        this.width = width != undefined ? width : this.width;
        this.height = height != undefined ? height : this.height;
        this.updateLTRB();
    }
    setXYWH(x, y, width, height) {

        this.x = x != undefined ? x : 0;
        this.y = y != undefined ? y : 0;
        this.width = width != undefined ? width : 0;
        this.height = height != undefined ? height : 0;

        this.updateLTRB();
    }

    setLT(l, t) {

        l = l != undefined ? l : this.x;
        t = t != undefined ? t : this.y;

        this.left = l;
        this.top = t;

        updateXYWH();
    }

    setRB(r, b) {

        r = r != undefined ? r : this.right;
        b = b != undefined ? b : this.bottom;

        this.right = r;
        this.bottom = b;

        updateXYWH();
    }

    setLTRB(l, t, r, b) {

        l = l != undefined ? l : this.x;
        t = t != undefined ? t : this.y;
        r = r != undefined ? r : this.right;
        b = b != undefined ? b : this.bottom;

        this.left = l;
        this.top = t;
        this.right = r;
        this.bottom = b;

        updateXYWH();
    }

    toZero() {

        this.x = this.left = 0;
        this.y = this.top = 0;
        this.right = 0;
        this.bottom = 0;
        this.width = 0;
        this.height = 0;
    }

    toEmpty() {

        this.x = this.left = 0xffffff;
        this.y = this.top = 0xffffff;
        this.right = -0xffffff;
        this.bottom = -0xffffff;
        this.width = 0;
        this.height = 0;
    }

    isEmpty() {
        if (this.width < 1)
            return true;
        if (this.height < 1)
            return true;
        return false;
    }

    updateXYWH() {

        if (this.right < this.left) {
            this.right = this.left;
        }
        if (this.bottom < this.top) {
            this.bottom = this.top;
        }
        this.x = this.left;
        this.y = this.top;
        this.width = this.right - this.x;
        this.height = this.bottom - this.y;
    }

    outset(dx, dy) {
        this.setLTRB(
            this.left - dx,
            this.top - dy,
            this.right + dx,
            this.bottom + dy
        );
    }
    updateLTRB() {
        this.left = this.x;
        this.top = this.y;
        this.right = this.x + this.width;
        this.bottom = this.y + this.height;
    }
    mapWithMat33To(mat3, dst) {
        dst.toEmpty();
        let pv = mat3.mapXY(this.left, this.top);
        dst.addXY(pv.x, pv.y);
        pv = mat3.mapXY(this.right, this.top);
        dst.addXY(pv.x, pv.y);
        pv = mat3.mapXY(this.right, this.bottom);
        dst.addXY(pv.x, pv.y);
        pv = mat3.mapXY(this.left, this.bottom);
        dst.addXY(pv.x, pv.y);
        dst.updateXYWH();
    }
    addXY(px, py) {
        if (this.left > px) this.left = px;
        if (this.right < px) this.right = px;
        if (this.top > py) this.top = py;
        if (this.bottom < py) this.bottom = py;
    }
    calcDistanceFrom(other) {
        let dx = Math.max(0, Math.max(other.x - this.right, this.x - other.right));
        let dy = Math.max(0, Math.max(other.y - this.bottom, this.y - other.bottom));
        return Math.sqrt(dx * dx + dy * dy);
    }
    contains(x, y) {
        if (x < this.left || x > this.right)
            return false;
        if (y < this.top || y > this.bottom)
            return false;
        return true;
    }
    intersects(other) {
        if (other.left > this.right || other.right < this.left)
            return false;
        if (other.y > this.bottom || other.bottom < this.y)
            return false;
        return true;
    }
}