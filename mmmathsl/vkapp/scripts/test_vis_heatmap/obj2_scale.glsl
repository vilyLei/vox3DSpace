float scaleScript(float t) {
    float px = 0;
    float py = -0.5;
    float hx = 0.5 * sin(t * 0.7);
    float hy = 0.5 * cos(t * 0.5);
    float d = sqrt((px - hx)*(px - hx) + (py - hy)*(py - hy));
    float heat = clamp(1.0 - d * 1.5, 0.0, 1.0);
    return 0.5 + 0.5 * heat;
}
