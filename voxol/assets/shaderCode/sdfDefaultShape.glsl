#version 330 core
out vec4 fragColor;
in vec2 v_uv;

uniform vec4 u_color;

float aaWithBlur(float d, float blur) {
    float aa = fwidth(d);
    return smoothstep(aa + blur, -aa - blur, d);
}

float sdfCircle(vec2 p, float r) {
    return length(p) - r;
}

float roundRect(vec2 pv, vec2 b, vec4 r)
{
    r.xy = (pv.x > 0.0) ? r.xy : r.zw;
    r.x  = (pv.y > 0.0) ? r.x  : r.y;
    vec2 q = abs(pv) - b + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}

#define SHAPE_STROKE = 1

#ifdef SHAPE_GLOW
void main() {
    float strokeWidth = 0.1;
    vec4 strokeColor = vec4(0.5, 0.0, 0.5, 1.0);
    vec2 center = vec2(0.5, 0.5);
    // float d = sdfCircle(v_uv - center, 0.3);
    float d = roundRect(v_uv - center, vec2(0.35, 0.35), vec4(0.1, 0.2, 0.0, 0.3));
    float aa = fwidth(d);
    // 填充过渡
    float fillAlpha = smoothstep(aa, -aa, d);

    // 描边区域 = 距离为 strokeWidth 附近的带宽
    float edgeStart = strokeWidth + aa;
    float edgeEnd = -aa;
    float strokeAlpha = smoothstep(edgeStart, edgeEnd, abs(d));

    float totalAlpha = max(fillAlpha, strokeAlpha);
    vec3 color = mix(u_color.rgb, strokeColor.rgb, strokeAlpha * (1.0 - fillAlpha));

    fragColor = vec4(color, totalAlpha);
}
#endif
#ifdef SHAPE_STROKE

void main() {

    vec2 center = vec2(0.5, 0.5);
    float d = roundRect(v_uv - center, vec2(0.35, 0.35), vec4(0.1, 0.2, 0.0, 0.3));
    
    vec4 strokeColor = vec4(0.9, 0.1, 0.9, 1.0);

    // 定义描边参数
    float strokeWidth = 0.05;
    
    float aa = fwidth(d);
    // 形状的内部（填充部分）
    float shape = smoothstep(0.0, aa, -d);
    // 形状的描边（外部部分）
    // float outline = smoothstep(strokeWidth, strokeWidth + aa, d);
    float outline = smoothstep(strokeWidth - aa, strokeWidth + aa, d);
    // 将描边和填充混合
    vec4 color = mix(strokeColor, u_color, shape);
    float alpha = (1.0 - outline) * color.a;
    float factor = step(1e-5, strokeWidth);
    fragColor = vec4(color.rgb * alpha, alpha) * factor;
    //fragColor += (1.0 - factor);
}

#endif