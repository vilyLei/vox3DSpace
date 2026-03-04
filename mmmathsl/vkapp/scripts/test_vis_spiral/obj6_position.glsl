vec2 positionScript(float t) {
    float ts = mod(t * 0.3 + 3.927, 12.566);
    float r = ts * 0.06;
    float x = r * cos(ts);
    float y = r * sin(ts);
    return vec2(x, y);
}
