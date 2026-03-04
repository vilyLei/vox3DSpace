vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.0381 * fade, 0.8966 * fade, 0.5652 * fade, 1.0);
}
