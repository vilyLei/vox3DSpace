vec4 colorScript(float t) {
    float v = 0.5 + 0.5 * sin(t * 2.0);
    return vec4(v, v, 1.0, 1.0);
}
