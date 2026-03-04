vec2 positionScript(float t) {
    float y = 0.6 - 1.2 * abs(sin(t * 2.0));
    return vec2(0.0, y);
}
