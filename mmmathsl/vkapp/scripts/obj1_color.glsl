vec4 colorScript(float t) {
    float r = 0.5 + 0.5 * sin(t);
    float g = 0.5 + 0.5 * sin(t + 2.094);
    float b = 0.5 + 0.5 * sin(t + 4.189);
    return vec4(r, g, b, 1.0);
}
