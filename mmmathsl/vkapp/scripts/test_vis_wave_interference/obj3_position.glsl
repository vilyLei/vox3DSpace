vec2 positionScript(float t) {
    float x = 0;
    float w1 = 0.5 * sin(2.0 * x - 1.5 * t);
    float w2 = 0.4 * sin(3.0 * x - 2.5 * t + 1.047);
    float y = (w1 + w2) * 0.5;
    return vec2(0.0, y);
}
