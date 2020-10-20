#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 v_TexCoord;
layout (location = 3) in float v_light;
layout (location = 4) in vec3 v_Tangent;

out vec4 vColor;
out vec2 a_TexCoord;
out vec3 norm;
out vec3 toCam;
out vec3 a_Tangent;
out vec3 a_Bitangent;
out mat3 toTangentSpace;
out vec3 v_toLightVec;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 toLightVec;
uniform mat4 toLightSpace;

void main() {
	vec4 worldPos = model * vec4(position, 1.0f);
	FragPos = worldPos.xyz;
	FragPosLightSpace = toLightSpace * worldPos;
	vColor = vec4(v_light, v_light, v_light, 1.0);
	a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
	gl_Position = proj * view * worldPos;

	norm = normalize(normal);
	a_Tangent = normalize(v_Tangent);
	a_Bitangent = normalize(cross(norm, a_Tangent));
	toTangentSpace = mat3 (
		a_Tangent.x, a_Bitangent.x, norm.x,
		a_Tangent.y, a_Bitangent.y, norm.y,
		a_Tangent.z, a_Bitangent.z, norm.z
	);
	toCam = /* toTangentSpace * */ ((inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - worldPos.xyz);
	v_toLightVec = /* toTangentSpace *  */toLightVec;
}
