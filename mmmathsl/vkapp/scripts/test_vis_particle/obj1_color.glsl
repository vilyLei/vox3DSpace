vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.5 * fade, 0.9331 * fade, 0.0669 * fade, 1.0);
}
