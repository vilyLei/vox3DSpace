float scaleScript(float t) {
    float ts = mod(t * 0.3 + 0, 12.566);
    float progress = ts / 12.566;
    return 0.3 + 0.7 * (1.0 - progress);
}
