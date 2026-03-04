float scaleScript(float t) {
    float envelope = exp(-0.15 * t);
    return 0.5 + 0.5 * envelope;
}
