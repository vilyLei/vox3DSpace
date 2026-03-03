vec2 positionScript(float t) {
    float x = 0.25 * sin(2.0 * t);
    float y = 0.25 * sin(3.0 * t + 1.571);
    return vec2(x, y);
}
