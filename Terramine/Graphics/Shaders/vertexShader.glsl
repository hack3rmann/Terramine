#version 330 core

layout (location = 0) in vec3 position;
out vec4 vColor;

void main() {
	vColor = vec4(0.0, 1.0, 0.0, 1.0);
	gl_Position = vec4(position, 1.0);
}
