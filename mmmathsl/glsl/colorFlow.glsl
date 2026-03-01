#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTex;

uniform float uExposureEV;
uniform float uContrast;
uniform float uSaturation;
uniform float uTemperature;
uniform float uTint;
uniform float uHighlights;
uniform float uShadows;
uniform float uHue; // radians

vec3 srgbToLinear(vec3 c) {
    c = clamp(c, 0.0, 1.0);
    vec3 lo = c / 12.92;
    vec3 hi = pow((c + 0.055) / 1.055, vec3(2.4));
    vec3 m  = vec3(1.0) - step(vec3(0.04045), c);
    return mix(hi, lo, m);
}
vec3 linearToSrgb(vec3 c) {
    c = max(c, 0.0);
    vec3 lo = c * 12.92;
    vec3 hi = 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055;
    vec3 m  = vec3(1.0) - step(vec3(0.0031308), c);
    return clamp(mix(hi, lo, m), 0.0, 1.0);
}
float srgbToLinear1(float x) { return srgbToLinear(vec3(x)).x; }
float linearToSrgb1(float x) { return linearToSrgb(vec3(x)).x; }

vec3 acesFitted(vec3 x) {
    const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float luma709(vec3 rgb) { return dot(rgb, vec3(0.2126, 0.7152, 0.0722)); }
float luminanceLin(vec3 lin) { return luma709(lin); }

vec3 applyExposure(vec3 lin, float ev) { return lin * exp2(ev); }

float contrastSigmoid(float x, float c) {
    x = clamp(x, 1e-5, 1.0 - 1e-5);
    float l = log(x / (1.0 - x));
    return 1.0 / (1.0 + exp(-l * c));
}

vec3 applyShadowsHighlightsAndContrast(vec3 lin, float sh, float hi, float c) {
    float l  = luminanceLin(lin);
    float lp = linearToSrgb1(l);

    float shadowRange = 0.65;
    float highlightRange = 0.65;
    float shadowMask    = smoothstep(1.0, 0.0, lp / shadowRange);
    float highlightMask = smoothstep(0.0, 1.0, (lp - (1.0 - highlightRange)) / highlightRange);

    float shAmt = clamp(abs(sh), 0.0, 1.0);
    float hiAmt = clamp(abs(hi), 0.0, 1.0);

    float shExp = exp2(-sh * 2.0);
    float hiExp = exp2( hi * 2.0);

    float lpSh = pow(lp, shExp);
    float lpHi = 1.0 - pow(1.0 - lp, hiExp);

    float lp2 = lp;
    lp2 = mix(lp2, lpSh, shadowMask * shAmt);
    lp2 = mix(lp2, lpHi, highlightMask * hiAmt);

    lp2 = contrastSigmoid(lp2, max(c, 1e-3));

    float l2 = srgbToLinear1(lp2);
    float scale = l2 / max(l, 1e-5);
    return lin * scale;
}

// Bradford WB
const mat3 M_RGB2XYZ = mat3(
    0.4124564, 0.2126729, 0.0193339,
    0.3575761, 0.7151522, 0.1191920,
    0.1804375, 0.0721750, 0.9503041
);
const mat3 M_XYZ2RGB = mat3(
     3.2404542, -0.9692660,  0.0556434,
    -1.5371385,  1.8760108, -0.2040259,
    -0.4985314,  0.0415560,  1.0572252
);
const mat3 M_BFD = mat3(
     0.8951, -0.7502,  0.0389,
     0.2664,  1.7135, -0.0685,
    -0.1614,  0.0367,  1.0296
);
const mat3 M_BFD_INV = mat3(
     0.9869929,  0.4323053, -0.0085287,
    -0.1470543,  0.5183603,  0.0400428,
     0.1599627,  0.0492912,  0.9684867
);

float temperatureToCCT(float t) {
    float mired0 = 1e6 / 6500.0;
    float mired  = mired0 + t * 80.0;
    float T = 1e6 / mired;
    return clamp(T, 2000.0, 25000.0);
}
vec2 cctToXy(float T) {
    T = clamp(T, 1667.0, 25000.0);
    float x;
    if (T <= 4000.0) x = -0.2661239e9/(T*T*T) - 0.2343580e6/(T*T) + 0.8776956e3/T + 0.179910;
    else            x = -3.0258469e9/(T*T*T) + 2.1070379e6/(T*T) + 0.2226347e3/T + 0.240390;

    float y;
    if (T <= 2222.0)      y = -1.1063814*x*x*x - 1.34811020*x*x + 2.18555832*x - 0.20219683;
    else if (T <= 4000.0) y = -0.9549476*x*x*x - 1.37418593*x*x + 2.09137015*x - 0.16748867;
    else                  y =  3.0817580*x*x*x - 5.87338670*x*x + 3.75112997*x - 0.37001483;
    return vec2(x, y);
}
vec2 xyToUv(vec2 xy) {
    float x = xy.x, y = xy.y;
    float d = (-2.0*x + 12.0*y + 3.0);
    return vec2(4.0*x/d, 6.0*y/d);
}
vec2 uvToXy(vec2 uv) {
    float u = uv.x, v = uv.y;
    float d = (2.0*u - 8.0*v + 4.0);
    return vec2((3.0*u)/d, (2.0*v)/d);
}
vec3 xyToXYZ(vec2 xy) {
    float x = xy.x, y = max(xy.y, 1e-6);
    return vec3(x/y, 1.0, (1.0 - x - y)/y);
}
mat3 bradfordAdapt(vec3 srcW, vec3 dstW) {
    vec3 srcLMS = M_BFD * srcW;
    vec3 dstLMS = M_BFD * dstW;
    vec3 s = dstLMS / max(srcLMS, vec3(1e-6));
    mat3 D = mat3(
        s.x, 0.0, 0.0,
        0.0, s.y, 0.0,
        0.0, 0.0, s.z
    );
    return M_BFD_INV * D * M_BFD;
}
vec3 applyWhiteBalanceBradford(vec3 linRGB, float temp, float ti) {
    float T = temperatureToCCT(temp);
    vec2 xy = cctToXy(T);

    vec2 uv = xyToUv(xy);
    uv.y += ti * 0.05;
    xy = uvToXy(uv);

    vec3 srcW = xyToXYZ(vec2(0.3127, 0.3290));
    vec3 dstW = xyToXYZ(xy);

    mat3 A = bradfordAdapt(srcW, dstW);

    vec3 xyz = M_RGB2XYZ * linRGB;
    xyz = A * xyz;
    return M_XYZ2RGB * xyz;
}

// Hue/Sat (YIQ) + hue-preserving gamut map
vec3 rgbToYIQ(vec3 rgb) {
    float y = dot(rgb, vec3(0.299, 0.587, 0.114));
    float i = dot(rgb, vec3(0.596, -0.274, -0.322));
    float q = dot(rgb, vec3(0.211, -0.523, 0.312));
    return vec3(y, i, q);
}
vec3 yiqToRgb(vec3 yiq) {
    float y=yiq.x,i=yiq.y,q=yiq.z;
    return vec3(
        y + 0.956*i + 0.621*q,
        y - 0.272*i - 0.647*q,
        y - 1.106*i + 1.703*q
    );
}
vec3 applyHueSatYIQ(vec3 rgb, float hueRad, float sat) {
    vec3 yiq = rgbToYIQ(rgb);
    yiq.yz *= sat;
    float ca = cos(hueRad), sa = sin(hueRad);
    float i2 = ca*yiq.y - sa*yiq.z;
    float q2 = sa*yiq.y + ca*yiq.z;
    yiq.y=i2; yiq.z=q2;
    return yiqToRgb(yiq);
}
vec3 gamutMapPreserveHue(vec3 rgb) {
    float l = luma709(rgb);
    vec3 d = rgb - vec3(l);

    float k = 1.0;
    if (d.r > 0.0) k = min(k, (1.0 - l) / d.r);
    if (d.g > 0.0) k = min(k, (1.0 - l) / d.g);
    if (d.b > 0.0) k = min(k, (1.0 - l) / d.b);
    if (d.r < 0.0) k = min(k, (0.0 - l) / d.r);
    if (d.g < 0.0) k = min(k, (0.0 - l) / d.g);
    if (d.b < 0.0) k = min(k, (0.0 - l) / d.b);

    k = clamp(k, 0.0, 1.0);
    return vec3(l) + d * k;
}

void main() {
    vec4 c0 = texture(uTex, vUV); // straight alpha

    vec3 lin = srgbToLinear(c0.rgb);
    lin = applyExposure(lin, uExposureEV);
    lin = applyWhiteBalanceBradford(lin, uTemperature, uTint);
    lin = applyShadowsHighlightsAndContrast(lin, uShadows, uHighlights, uContrast);

    lin = max(lin, vec3(0.0));
    lin = min(lin, vec3(64.0));
    lin = acesFitted(lin);

    vec3 srgb = linearToSrgb(lin);
    srgb = applyHueSatYIQ(srgb, uHue, uSaturation);
    srgb = gamutMapPreserveHue(srgb);
    srgb = clamp(srgb, 0.0, 1.0);

    FragColor = vec4(srgb, c0.a);
}