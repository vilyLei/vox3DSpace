vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0 * fade, 0.7498 * fade, 0.7499 * fade, 1.0);
}
