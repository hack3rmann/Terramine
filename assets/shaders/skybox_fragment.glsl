#version 450 core

in vec2 v_uv;
out vec4 result_color;

uniform sampler2D skybox_texture;

void main() {
    result_color = texture(skybox_texture, v_uv);
}
