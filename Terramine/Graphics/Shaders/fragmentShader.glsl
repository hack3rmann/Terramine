#version 330 core

in vec4 vColor;
in vec2 a_TexCoord;
out vec4 color;

uniform sampler2D u_Texture0;
uniform vec2 resolution;

void main() {
	vec4 col = vec4(0.50,0.65,0.84, 1.0f);
	if (gl_FragCoord.x >= resolution.x / 2 - 1 &&
		gl_FragCoord.x <= resolution.x / 2 + 1 &&
		gl_FragCoord.y >= resolution.y / 2 - 8 &&
		gl_FragCoord.y <= resolution.y / 2 + 8 ||
		gl_FragCoord.x >= resolution.x / 2 - 8 &&
		gl_FragCoord.x <= resolution.x / 2 + 8 &&
		gl_FragCoord.y >= resolution.y / 2 - 1 &&
		gl_FragCoord.y <= resolution.y / 2 + 1) {
		vec4 texColor = col * texture(u_Texture0, a_TexCoord);
		vec3 i = vec3((texColor.r + texColor.g + texColor.b) / 3);
		color = vec4(vec3(1.0f - i) * 2.0f, 1.0f);
	} else {
		vec4 texColor = col * texture(u_Texture0, a_TexCoord);
		color = texColor;
	}
}
