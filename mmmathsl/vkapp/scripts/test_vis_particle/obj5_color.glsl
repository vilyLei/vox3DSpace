vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(1 * fade, 0.2502 * fade, 0.2501 * fade, 1.0);
}
