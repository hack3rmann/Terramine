#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 v_TexCoord;
layout (location = 2) in float v_light;

out vec4 vColor;
out vec2 a_TexCoord;

uniform mat4 model;
uniform mat4 projView;

void main() {
	vColor = vec4(v_light, v_light, v_light, 1.0);
	a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
	gl_Position = projView * model * vec4(position, 1.0);
}
