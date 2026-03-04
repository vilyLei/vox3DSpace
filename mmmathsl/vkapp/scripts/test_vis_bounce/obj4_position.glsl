vec2 positionScript(float t) {
    float b = abs(sin(t * 2.0));
    float y = -0.6 + 1.2 * b * b;
    return vec2(0.0, y);
}
