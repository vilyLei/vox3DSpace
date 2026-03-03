vec4 colorScript(float t) {
    float h = t * 0.1;
    float r = 0.5 + 0.5 * sin(h * 6.283);
    float g = 0.5 + 0.5 * sin(h * 6.283 + 2.094);
    float b = 0.5 + 0.5 * sin(h * 6.283 + 4.189);
    return vec4(r, g, b, 1.0);
}
