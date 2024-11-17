#version 450 core

in float v_light;
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_to_camera;
in vec3 v_to_light;

out vec4 result_color;

uniform sampler2D albedo_texture;
uniform sampler2D normal_texture;
uniform vec2 resolution;
uniform vec3 light_color;

void main() {
    vec4 albedo = texture(albedo_texture, v_uv);
    result_color = vec4(v_light * albedo.rgb, albedo.a);
}
