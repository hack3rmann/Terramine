#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float data_pack;

out float a_light;
out vec2 a_TexCoord;
out vec3 norm;
out vec3 toCam;
out vec3 v_toLightVec;
out vec2 texture_index;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 toLightVec;
uniform mat4 toLightSpace;

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
    out vec3 normal,
    out vec2 uv
) {
    uint n_normal_bits = 3u;
    uint normal_mask = (1u << n_normal_bits) - 1u;
    uint n_texcoord_bits = 4u;
    uint texcoord_mask = (1u << n_texcoord_bits) - 1u;
    uint corner_index_mask = 3u;

    uint normal_index = normal_mask & (pack >> 0u);
    uint v = texcoord_mask & (pack >> (n_normal_bits + 0u * n_texcoord_bits));
    uint u = texcoord_mask & (pack >> (n_normal_bits + 1u * n_texcoord_bits));
    uint corner_index = corner_index_mask & (pack >> (n_normal_bits + 2u * n_texcoord_bits));

    normal = normals[normal_index];
    uv = 0.0625 * vec2(
                float(u + 1u - (1u & (corner_index >> 1u))),
                float(v + 1u - (1u & (corner_index >> 0u)))
            );
}

void main() {
    vec2 v_TexCoord;
    unpack_data(floatBitsToUint(data_pack), norm, v_TexCoord);

    vec4 worldPos = vec4(position, 1.0);
    float v_light = 1.0;
    a_light = v_light;
    a_TexCoord = vec2(v_TexCoord.x, v_TexCoord.y);
    gl_Position = proj * view * worldPos;

    toCam = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;
    v_toLightVec = toLightVec;
}
