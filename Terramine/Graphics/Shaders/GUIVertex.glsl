#version 330 core

layout (location = 0) in vec2 gui_position;
layout (location = 1) in vec2 v_gui_TexCoord;
out vec2 a_gui_TexCoord;

uniform mat4 modelProj;

void main() {
	a_gui_TexCoord = vec2(v_gui_TexCoord.x, 1 - v_gui_TexCoord.y);
	gl_Position = modelProj * vec4(gui_position, 0.0f, 1.0f);
}