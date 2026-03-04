vec2 positionScript(float t) {
    float r = mod(t * 0.4, 1.0) * 0.8;
    return vec2(-0.9239 * r, -0.3827 * r);
}
