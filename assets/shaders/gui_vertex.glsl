#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 v_uv;

uniform mat4 model_projection;

void main() {
    v_uv = vec2(uv.x, 1.0 - uv.y);
    gl_Position = model_projection * vec4(position, 0.0f, 1.0f);
}

