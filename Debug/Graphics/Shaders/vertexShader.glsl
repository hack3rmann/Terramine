#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 v_TexCoord;
layout (location = 3) in float v_light;

out vec4 vColor;
out vec2 a_TexCoord;
out vec3 surfNorm;
out vec3 toCam;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	vec4 worldPos = model * vec4(position, 1.0f);
	vColor = vec4(v_light, v_light, v_light, 1.0);
	a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
	surfNorm = (vec4(normal, 1.0f)).xyz;
	toCam = (inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - worldPos.xyz;
	gl_Position = proj * view * worldPos;
}
