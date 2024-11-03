#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 v_TexCoord;

out vec4 vColor;
out vec2 a_TexCoord;

uniform mat4 projView;
uniform vec3 camPos;

void main() {
	vColor = vec4(1.0);
	a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
	gl_Position = projView * vec4((position + camPos) * 1.0f, 1.0);
}
