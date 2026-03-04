vec4 colorScript(float t) {
    float s = 0.5 + 0.5 * sin(t * 2.0 + 4.7124);
    return vec4(1.0, 1.0, 0.3, 1.0) * (0.5 + 0.5 * s);
}
