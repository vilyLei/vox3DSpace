vec2 positionScript(float t) {
    float s = sin(t * 2.0 + 3.1416);
    float y = -0.15 * s;
    return vec2(0.0, y);
}
