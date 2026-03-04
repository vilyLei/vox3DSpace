vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.1464 * fade, 0.9829 * fade, 0.3705 * fade, 1.0);
}
