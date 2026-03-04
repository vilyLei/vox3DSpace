vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.6913 * fade, 0.0043 * fade, 0.8045 * fade, 1.0);
}
