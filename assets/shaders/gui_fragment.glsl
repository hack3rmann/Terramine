#version 450 core

in vec2 v_uv;

out vec4 result_color;

uniform sampler2D gui_texture;

void main() {
    result_color = texture(gui_texture, v_uv);

    if (result_color.a == 0.0f) {
        discard;
    }
}
