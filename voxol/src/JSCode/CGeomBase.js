"use strict";

export class ViewBounds {

    constructor(x, y, width, height) {

        this.setXYWH(x, y, width, height);
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
        this.width = width != undefined ? width : 256;
        this.height = height != undefined ? height : 256;

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

        l = l != undefined ? l : this.x;
        t = t != undefined ? t : this.y;

        this.left = l;
        this.top = t;

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
        this.y = this.right = 0;
        this.top = 0;
        this.bottom = 0;
        this.width = 0;
        this.height = 0;
    }

    toEmpty() {

        this.x = this.left = 0xffffff;
        this.y = this.right = -0xffffff;
        this.top = 0xffffff;
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
    scaleAndTranslateTo(sx, sy, tx, ty) {

    }
}