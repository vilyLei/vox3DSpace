vec4 colorScript(float t) {
    float envelope = exp(-0.4 * t);
    float brightness = 0.3 + 0.7 * envelope;
    return vec4(0.3, 0.3, 1.0, 1.0) * brightness;
}
