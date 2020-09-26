#version 330 core

in vec4 vColor;
in vec2 a_TexCoord;
out vec4 color;

uniform sampler2D u_Texture0;
uniform vec2 resolution;

void main() {
	if (gl_FragCoord.x >= resolution.x / 2 - 1 && gl_FragCoord.x <= resolution.x / 2 + 1 && gl_FragCoord.y >= resolution.y / 2 - 8 && gl_FragCoord.y <= resolution.y / 2 + 8 ||
		gl_FragCoord.x >= resolution.x / 2 - 8 && gl_FragCoord.x <= resolution.x / 2 + 8 && gl_FragCoord.y >= resolution.y / 2 - 1 && gl_FragCoord.y <= resolution.y / 2 + 1)
		color = vec4(vec3(1.0) - (vColor * texture(u_Texture0, a_TexCoord)).rgb, 1.0);
	else {
		vec4 texColor = vColor * texture(u_Texture0, a_TexCoord);
		color = texColor;
	}
}
