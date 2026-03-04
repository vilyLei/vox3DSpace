float scaleScript(float t) {
    float T = 3.3333;
    float ts = mod(t, T);
    float progress = ts / T;
    float sqash = 1.0 - 0.3 * sin(3.14159 * progress);
    return sqash;
}
