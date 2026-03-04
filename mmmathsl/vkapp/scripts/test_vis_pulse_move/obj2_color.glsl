vec4 colorScript(float t) {
    float s = 0.5 + 0.5 * sin(t * 2.0 + 1.5708);
    return vec4(0.3, 1.0, 0.3, 1.0) * (0.5 + 0.5 * s);
}
