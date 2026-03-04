vec2 positionScript(float t) {
    float T = 2.6667;
    float v0 = 1.6;
    float g = 1.2;
    float floor_y = -0.7;
    float ts = mod(t, T);
    float y = floor_y + v0 * ts - 0.5 * g * ts * ts;
    return vec2(0.0, y);
}
