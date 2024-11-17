#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

uniform mat4 projection_view;
uniform vec3 camera_position;

void main() {
    v_uv = vec2(uv.x, 1.0 - uv.y);
    gl_Position = projection_view * vec4(position + camera_position, 1.0);
}
