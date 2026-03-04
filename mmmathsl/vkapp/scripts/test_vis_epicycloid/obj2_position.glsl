vec2 positionScript(float t) {
    float x = 0.13334 * cos(t) - 0.06667 * cos(2 * t);
    float y = 0.13334 * sin(t) - 0.06667 * sin(2 * t);
    return vec2(x, y);
}
