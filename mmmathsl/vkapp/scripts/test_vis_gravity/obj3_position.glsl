vec2 positionScript(float t) {
    float T = 3.3333;
    float v0 = 2;
    float g = 1.2;
    float floor_y = -0.7;
    float ts = mod(t, T);
    float y = floor_y + v0 * ts - 0.5 * g * ts * ts;
    return vec2(0.0, y);
}
