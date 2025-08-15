"use strict";
export class TextureBuilder {

    static createTextureFromImage(gl, image) {

        // const tex = gl.createTexture();
        // gl.bindTexture(gl.TEXTURE_2D, tex);
        // gl.texImage2D(
        //     gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA,
        //     gl.UNSIGNED_BYTE, image
        // );
        // // settine params
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        // gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        // gl.generateMipmap(gl.TEXTURE_2D);

        const tex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, tex);

        gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);

        // if ((image.width & (image.width - 1)) === 0 && (image.height & (image.height - 1)) === 0) {
        //     gl.generateMipmap(gl.TEXTURE_2D);
        // } else {
        //     gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        // }
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

            
        // fetch(url)
        // .then(res => res.blob())
        // .then(blob => createImageBitmap(blob, {
        //     premultiplyAlpha: "none",  // core
        //     colorSpaceConversion: "none"
        // }))
        // .then(bitmap => {
        //     const tex = TextureBuilder.createTextureFromImage(gl, bitmap);
        //     callback(tex, id);
        // });
    }
}