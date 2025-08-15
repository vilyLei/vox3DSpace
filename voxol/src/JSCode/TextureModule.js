"use strict";
export class TextureBuilder {

    static createTextureFromImage(gl, image) {
        const tex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, tex);

        gl.texImage2D(
            gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA,
            gl.UNSIGNED_BYTE, image
        );

        // settine params
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.generateMipmap(gl.TEXTURE_2D);

        return tex;
    }

    static loadImageAndCreateTexture(gl, url, id, callback) {

        fetch(url)
            .then(res => res.blob())
            .then(blob => createImageBitmap(blob))
            .then(bitmap => {
                const tex = TextureBuilder.createTextureFromImage(gl, bitmap);
                callback(tex, id);
            });

        // const img = new Image();
        // img.onload = () => {
        //     const tex = TextureBuilder.createTextureFromImage(gl, img);
        //     callback(tex, id);
        // };
        // img.src = url;
    }
}