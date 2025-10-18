#version 330 core
out vec4 fragColor;
in vec2 v_uv;
uniform vec4 u_color;
float u_time = 0.7;

// ----------------------------
// 📦 基础 SDF 函数定义
// ----------------------------

// 圆形
float sdCircle(vec2 p, float r) {
    return length(p) - r;
}

// 矩形（box）
float sdBox(vec2 p, vec2 b) {
    vec2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// 胶囊（圆头线段）
float sdCapsule(vec2 p, vec2 a, vec2 b, float r) {
    vec2 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h) - r;
}

// 等腰三角形（常用于喷口）
float sdTriangleIsosceles(vec2 p, vec2 q) {
    // q.x 为底宽的一半，q.y 为高度
    p.x = abs(p.x);
    vec2 a = p - q * clamp(dot(p, q) / dot(q, q), 0.0, 1.0);
    vec2 b = p - vec2(q.x * clamp(p.x / q.x, 0.0, 1.0), q.y);
    float s = (b.y < 0.0 && a.y < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(dot(a, a), dot(b, b)));
}

// Smooth union
float opSmoothUnion(float d1, float d2, float k) {
    float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

// ----------------------------
// 🚀 飞船造型定义
// ----------------------------
float sdfShip(vec2 p) {
    // 调整坐标比例
    p.y *= 1.2;

    // 主体：椭圆体
    float body = sdCircle(p / vec2(0.6, 1.0), 0.6);

    // 左右机翼
    float wingL = sdBox(p - vec2(-0.7, 0.0), vec2(0.3, 0.1));
    float wingR = sdBox(p - vec2( 0.7, 0.0), vec2(0.3, 0.1));

    // 喷口底部
    float thruster = sdTriangleIsosceles(p - vec2(0.0, -0.9), vec2(0.25, 0.25));

    // 舱盖
    float dome = sdCircle(p - vec2(0.0, 0.55), 0.25);

    // 合并
    float ship = opSmoothUnion(body, wingL, 0.15);
    ship = opSmoothUnion(ship, wingR, 0.15);
    ship = opSmoothUnion(ship, thruster, 0.15);
    ship = opSmoothUnion(ship, dome, 0.10);

    return ship;
}

// ----------------------------
// 🌈 主片段逻辑
// ----------------------------
void main() {
    // 将 UV 映射到 [-1,1] 空间
    vec2 p = (v_uv * 2.0 - 1.0);

    // 飞船缓慢漂浮动画
    p.y += sin(u_time * 1.5) * 0.05;

    // 呼吸缩放
    float scale = 1.0 + 0.05 * sin(u_time * 2.0);
    p /= scale;

    float d = sdfShip(p);

    // 颜色设定
    vec3 baseColor = vec3(0.3, 0.6, 1.0);
    vec3 edgeColor = vec3(0.1, 0.2, 0.4);
    vec3 glowColor = vec3(1.0, 0.8, 0.4);

    // 描边计算
    float edge = smoothstep(0.005, 0.03, abs(d));

    // 基础填色
    vec3 color = mix(baseColor, edgeColor, edge);

    // 喷口发光区域
    float flame = smoothstep(0.15, 0.0, length(p - vec2(0.0, -1.0)));
    flame *= (0.8 + 0.3 * sin(u_time * 10.0 + p.y * 20.0));
    color += glowColor * flame * 0.8;

    // Alpha
    float alpha = 1.0 - smoothstep(0.02, 0.05, d);
    alpha *= u_color.a;
    fragColor = vec4(color * u_color.rgb * alpha, alpha);
}
