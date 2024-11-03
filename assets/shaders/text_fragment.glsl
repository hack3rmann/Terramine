#version 330 core

in vec2 a_TexCoord;
in vec3 a_Color;

out vec4 color;

uniform sampler2D u_Texture0;

void main() {
	color = vec4(a_Color, 1.0f) * texture(u_Texture0, a_TexCoord);
}