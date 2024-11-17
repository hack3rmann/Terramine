#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in float float_color_pack;

out vec4 v_color;

uniform mat4 projection_view;

void main() {
    uint color_pack = floatBitsToInt(float_color_pack);

    v_color = vec4(
            (color_pack >> 0) & 255,
            (color_pack >> 8) & 255,
            (color_pack >> 16) & 255,
            (color_pack >> 24) & 255
        ) / 255.0;

    gl_Position = projection_view * vec4(position, 1.0f);
}
