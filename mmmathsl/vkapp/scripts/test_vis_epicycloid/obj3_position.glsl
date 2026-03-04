vec2 positionScript(float t) {
    float x = 0.08 * cos(t) - 0.04 * cos(2 * t);
    float y = 0.08 * sin(t) - 0.04 * sin(2 * t);
    return vec2(x, y);
}
