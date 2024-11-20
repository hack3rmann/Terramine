#version 450 core

in vec2 v_uv;
out vec4 result_color;

uniform sampler2D depth_color_texture;
uniform sampler2D screen_color_texture;
uniform vec2 resolution;
uniform float scaling;

bool is_cross(float thickness, float size) {
    bool is_vertical = abs(gl_FragCoord.x - 0.5 * resolution.x) <= 0.5 * thickness
            && abs(gl_FragCoord.y - 0.5 * resolution.y) <= 0.5 * size;

    bool is_horizontal = abs(gl_FragCoord.x - 0.5 * resolution.x) <= 0.5 * size
            && abs(gl_FragCoord.y - 0.5 * resolution.y + 0.5) <= 0.5 * thickness;

    return is_vertical || is_horizontal;
}

float linearize_depth(float depth, float near, float far) {
    return near * far / (far + depth * (near - far));
}

void main() {
    vec4 screen_color = texture(screen_color_texture, v_uv);
    result_color = screen_color;

    if (is_cross(2.0 * scaling, 16.0 * scaling)) {
        result_color = vec4(1.0 - result_color.rgb, 1.0);
    }
}
