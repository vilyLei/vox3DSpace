vec2 positionScript(float t) {
    float x = 0.2 * cos(t) - 0.1 * cos(2 * t);
    float y = 0.2 * sin(t) - 0.1 * sin(2 * t);
    return vec2(x, y);
}
