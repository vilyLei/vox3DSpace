
float sdfCircleBase(float radius, vec2 center, vec2 xy) {
    return length(xy - center) - radius;
}
float sdfCircle(vec2 p, float r) {
    return length(p) - r;
}

float sdfRing(vec2 p, float radius, float thickness) {
    return abs(length(p) - radius) - thickness * 0.5;
}

float sdfRect(vec2 p, vec2 size) {
    vec2 d = abs(p) - size;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}
float sdfRoundRect(vec2 pv, vec2 b, vec4 r) {
    r.xy = (pv.x > 0.0) ? r.xy : r.zw;
    r.x = (pv.y > 0.0) ? r.x : r.y;
    vec2 q = abs(pv) - b + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}
float sdfRoundedRect(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size;
    return length(max(d, 0.0)) - radius;
}

// ---------------- Triangle ----------------
// Equilateral triangle centered at origin
float sdfTriangle(vec2 p, float size) {
    const float k = sqrt(3.0);
    p.x = abs(p.x) - size;
    p.y = p.y + size/k;
    if( p.x + k*p.y > 0.0 ) p = vec2(p.x - k*p.y, -k*p.x - p.y)/2.0;
    p.x -= clamp(p.x, -2.0*size, 0.0);
    return -length(p)*sign(p.y);
}


float sdfStar(vec2 p, float rOuter, float rInner, int n) {
    float angle = atan(p.y, p.x);
    float radius = length(p);
    float k = float(n)*0.5;
    float m = cos(mod(angle*k,3.14159265) - 3.14159265*0.5);
    float d = radius - mix(rOuter, rInner, m);
    return d;
}

float sdfSector(vec2 p, float radius, vec2 dirStart, vec2 dirEnd) {
    float len = length(p);
    float dRadius = len - radius;
    float sideStart = - (dirStart.x * p.y - dirStart.y * p.x); // cross(dirStart, p)
    float sideEnd   =   (dirEnd.x   * p.y - dirEnd.y   * p.x); // cross(dirEnd, p)
    float dAngle = max(sideStart, sideEnd);
    return max(dRadius, dAngle);
}

float intersect(float a, float b) { return max(a, b); }
float union(float a, float b) { return min(a, b); }
float subtract(float a, float b) { return max(a, -b); }

float smoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) - k*h*(1.0 - h);
}
float smoothSubtract(float d1, float d2, float k) {
    return smoothUnion(d1, -d2, k);
}
float smoothIntersect(float d1, float d2, float k) {
    return -smoothUnion(-d1, -d2, k);
}
float union4(float d0, float d1, float d2, float d3) {
    return min(min(d0, d1), min(d2, d3));
}
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}
float smoothUnion4(float d0, float d1, float d2, float d3, float k) {
    float d = smin(d0, d1, k);
    d = smin(d, d2, k);
    d = smin(d, d3, k);
    return d;
}

vec4 smoothUnionVec4(vec4 c1, vec4 c2, float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(c1, c2, h);
    //float fw = fwidth(h);
    //h = smoothstep(0.0 - fw, 1.0 + fw, h);
    //c1 = pow(c1, vec4(2.2));
    //c2 = pow(c2, vec4(2.2));
    //vec4 c = mix(c1, c2, h);
    //return pow(c, vec4(1.0 / 2.2));
}