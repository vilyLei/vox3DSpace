vec4 colorScript(float t) {
    float px = 0.5;
    float py = -0.5;
    float hx = 0.5 * sin(t * 0.7);
    float hy = 0.5 * cos(t * 0.5);
    float d = sqrt((px - hx)*(px - hx) + (py - hy)*(py - hy));
    float heat = clamp(1.0 - d * 1.5, 0.0, 1.0);
    float r = smoothstep(0.0, 0.5, heat);
    float g = smoothstep(0.25, 0.75, heat) * (1.0 - smoothstep(0.75, 1.0, heat));
    float b = 1.0 - smoothstep(0.0, 0.5, heat);
    return vec4(r, g, b, 1.0);
}
