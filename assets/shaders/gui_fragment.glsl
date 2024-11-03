#version 330 core

in vec2 a_gui_TexCoord;
out vec4 color;

uniform sampler2D u_Texture1;

void main() {
	color = texture(u_Texture1, a_gui_TexCoord);
	if (color.a == 0.0f)
		discard;
}
