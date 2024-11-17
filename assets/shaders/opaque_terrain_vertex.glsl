#version 450 core

layout(location = 0) in float position_pack;

out float v_light;
out vec2 v_uv;
out vec3 v_normal;
out vec3 v_to_camera;
out vec3 v_to_light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 to_light;

vec3 normals[] = vec3[](
        vec3(1.0, 0.0, 0.0),
        vec3(-1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, -1.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 0.0, -1.0)
    );

void unpack_data(
    uint pack,
    out vec3 position, out vec3 normal,
    out float light, out vec2 uv
) {
    uint n_pos_bits = 4u;
    uint pos_mask = (1u << n_pos_bits) - 1u;
    uint n_offset_bits = 3u;
    uint offset_mask = (1u << n_offset_bits) - 1u;
    uint n_normal_bits = 3u;
    uint normal_mask = (1u << n_normal_bits) - 1u;
    uint n_light_bits = 4u;
    uint light_mask = (1u << n_light_bits) - 1u;
    uint n_texcoord_bits = 4u;
    uint texcoord_mask = (1u << n_texcoord_bits) - 1u;
    uint corner_index_mask = 3u;

    uint x = pos_mask & (pack >> (0u * n_pos_bits));
    uint y = pos_mask & (pack >> (1u * n_pos_bits));
    uint z = pos_mask & (pack >> (2u * n_pos_bits));
    uint offset_bits = offset_mask & (pack >> (3u * n_pos_bits));
    uint normal_index = normal_mask & (pack >> (3u * n_pos_bits + n_offset_bits));
    uint light_bits = light_mask & (pack >> (3u * n_pos_bits + n_offset_bits + n_normal_bits));
    uint v = texcoord_mask
            & (pack >> (3u * n_pos_bits + n_offset_bits + n_normal_bits + n_light_bits + 0u * n_texcoord_bits));
    uint u = texcoord_mask
            & (pack >> (3u * n_pos_bits + n_offset_bits + n_normal_bits + n_light_bits + 1u * n_texcoord_bits));
    uint corner_index = corner_index_mask
            & (pack >> (3u * n_pos_bits + n_offset_bits + n_normal_bits + n_light_bits + 2u * n_texcoord_bits));

    position = vec3(
            float(x) + 0.5 - float(1u & (offset_bits >> 2u)),
            float(y) + 0.5 - float(1u & (offset_bits >> 1u)),
            float(z) + 0.5 - float(1u & (offset_bits >> 0u))
        );

    normal = normals[normal_index];
    light = float(light_bits) / float(light_mask);
    uv = 0.0625 * vec2(
                float(u + 1u - (1u & (corner_index >> 1u))),
                float(v + 1u - (1u & (corner_index >> 0u)))
            );
}

void main() {
    vec3 position;
    unpack_data(floatBitsToUint(position_pack), position, v_normal, v_light, v_uv);

    vec4 world_position = model * vec4(position, 1.0);

    v_to_camera = (inverse(view) * vec4(vec3(0.0), 1.0)).xyz - world_position.xyz;
    v_to_light = to_light;

    gl_Position = projection * view * world_position;
}
