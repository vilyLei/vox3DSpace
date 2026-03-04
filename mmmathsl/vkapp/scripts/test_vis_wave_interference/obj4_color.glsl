vec4 colorScript(float t) {
    float x = 0.3;
    float w1 = 0.5 + 0.5 * sin(2.0 * x - 1.5 * t);
    float w2 = 0.4 * sin(3.0 * x - 2.5 * t + 1.047);
    float amp = clamp(w1 + w2, -1.0, 1.0);
    float r = clamp(amp, 0.0, 1.0);
    float b = clamp(-amp, 0.0, 1.0);
    float g = 1.0 - abs(amp);
    return vec4(r, g, b, 1.0);
}
