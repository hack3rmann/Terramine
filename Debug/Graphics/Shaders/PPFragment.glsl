#version 330 core

in vec2 a_TexCoord;
out vec4 color;

uniform sampler2D screenColor;

void main() {
	color = texture(screenColor, a_TexCoord);
}