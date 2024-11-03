#version 330 core

in vec4 vColor;
in vec2 a_TexCoord;
out vec4 color;

uniform sampler2D u_Texture0;
uniform vec2 resolution;

void main() {
    vec4 texColor = vColor * texture(u_Texture0, a_TexCoord);
    color = texColor;
}
