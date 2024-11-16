#version 450 core

in vec4 v_color;
out vec4 out_color;

void main() {
    out_color = vec4(v_color.rgb, 1.0);
}
