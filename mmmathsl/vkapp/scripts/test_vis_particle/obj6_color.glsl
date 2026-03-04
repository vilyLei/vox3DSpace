vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.9619 * fade, 0.1034 * fade, 0.4348 * fade, 1.0);
}
