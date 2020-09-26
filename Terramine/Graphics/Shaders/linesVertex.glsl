#version 330 core
//#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

out vec4 v_Color;

uniform mat4 projView;

void main() {
	v_Color = a_color;
	gl_Position = projView * vec4(a_position, 1.0f);
}