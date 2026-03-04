vec2 positionScript(float t) {
    float envelope = exp(-0.15 * t);
    float y = 0.5 * envelope * cos(3 * t);
    return vec2(0.0, y);
}
