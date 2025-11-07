#version 330 core
precision mediump float;

uniform vec4      u_color;
uniform sampler2D u_tex0;
in vec2 v_uv;
out vec4 outColor;

vec2 uDirection = vec2(1.0, 0.0);       // (1.0, 0.0) = 水平模糊; (0.0, 1.0) = 垂直模糊
float uRadius = 4.0;                           // 模糊半径（例如 4.0）
float uSigma = 2.0;                            // 高斯分布σ（例如 2.0）
void main() {
    
    vec4 baseColor = texture(u_tex0, v_uv);

    vec2 texelSize = 0.5 / vec2(textureSize(u_tex0, 0));
    vec4 color = vec4(0.0);
    float total = 0.0;

    for (int i = -5; i <= 5; ++i)
    {
        float x = float(i);
        float weight = exp(-0.5 * (x * x) / (uSigma * uSigma));
        vec2 offset = uDirection * texelSize * x * uRadius;
        vec4 src = texture(u_tex0, v_uv + offset);
        src.rgb *= src.a;
        color += src * weight;
        total += weight;
    }
    color /= total;
    if(color.a > 0.0)
        color.rgb /= color.a;
    outColor = color * u_color;
}