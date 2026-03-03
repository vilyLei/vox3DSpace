#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec4 fragColorTint;
layout(location = 0) out vec4 outColor;

void main() {
    // Mix vertex color with animated tint
    vec3 finalColor = fragColor * fragColorTint.rgb;
    outColor = vec4(finalColor, 1.0);
}
