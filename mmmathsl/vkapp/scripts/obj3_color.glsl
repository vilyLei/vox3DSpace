vec4 colorScript(float t) {
    float r = 0.5 + 0.5 * sin(t * 1.5);
    float g = 0.5 + 0.5 * sin(t * 1.5 + 3.1416);
    return vec4(r, g, 0.2, 1.0);
}
