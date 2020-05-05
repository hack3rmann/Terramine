#version 330 core

attribute vec4 vPosition;
attribute vec3 vColor;
out vec4 color;

void main() {
	color = vec4(vColor, 1.0);
	gl_Position = vPosition;
}
