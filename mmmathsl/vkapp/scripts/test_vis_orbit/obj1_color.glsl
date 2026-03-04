vec4 colorScript(float t) {
    float pulse = 0.85 + 0.15 * sin(t * 3.0);
    return vec4(1.0, 0.9, 0.3, 1.0) * pulse;
}
