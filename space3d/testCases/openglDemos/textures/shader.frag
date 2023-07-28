#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture0;

void main()
{
	FragColor = texture(texture0, TexCoord);
}