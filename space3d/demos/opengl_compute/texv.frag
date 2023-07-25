#version 460 core

layout(binding = 0) uniform sampler2D tex;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  vec2 texSize = vec2(textureSize(tex, 0));
  vec2 sv2 = gl_FragCoord.xy /texSize.xy;
  fragColor = texture(tex, texCoords);
  fragColor.yz *= sv2.xy;
}