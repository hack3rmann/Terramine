#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 vColor;

out vec2 a_TexCoord;
out vec3 a_Color;

uniform mat4 modelProj;

void main() {
	gl_Position = modelProj * vec4(position, 0.0f, 1.0f);
	a_TexCoord = vec2(texCoords.x, 1 - texCoords.y);;
	//a_TexCoord = texCoords;
	a_Color = vColor;
}