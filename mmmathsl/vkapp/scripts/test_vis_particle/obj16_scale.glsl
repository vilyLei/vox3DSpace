float scaleScript(float t) {
    float r = mod(t * 0.4, 1.0);
    return 1.0 - r * 0.8;
}
