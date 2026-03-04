vec4 colorScript(float t) {
    float fade = 1.0 - mod(t * 0.4, 1.0);
    return vec4(0.3087 * fade, 0.9957 * fade, 0.1955 * fade, 1.0);
}
