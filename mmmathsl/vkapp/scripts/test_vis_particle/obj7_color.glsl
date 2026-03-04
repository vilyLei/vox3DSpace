vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.8536 * fade, 0.0171 * fade, 0.6295 * fade, 1.0);
}
