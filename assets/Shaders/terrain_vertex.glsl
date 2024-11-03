#version 330 core

layout(location = 0) in float position_pack;
layout(location = 1) in vec2 v_TexCoord;

out float a_light;
out vec2 a_TexCoord;
out vec3 norm;
out vec3 toCam;
out vec3 a_Tangent;
out vec3 a_Bitangent;
out mat3 toTangentSpace;
out vec3 v_toLightVec;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 toLightVec;
uniform mat4 toLightSpace;

void unpack_data(uint pack, out vec3 position, out vec3 normal, out vec3 tangent, out float light) {
    vec3 normals[] = vec3[](
            vec3(1.0, 0.0, 0.0),
            vec3(-1.0, 0.0, 0.0),
            vec3(0.0, 1.0, 0.0),
            vec3(0.0, -1.0, 0.0),
            vec3(0.0, 0.0, 1.0),
            vec3(0.0, 0.0, -1.0)
        );

    vec3 tangents[] = vec3[](
            vec3(0.0, 0.0, -1.0),
            vec3(0.0, 0.0, 1.0),
            vec3(1.0, 0.0, 0.0),
            vec3(1.0, 0.0, 0.0),
            vec3(1.0, 0.0, 0.0),
            vec3(-1.0, 0.0, 0.0)
        );

    uint n_bits = 4u;
    uint pos_mask = (1u << n_bits) - 1u;
    uint n_offset_bits = 3u;
    uint offset_mask = (1u << n_offset_bits) - 1u;
    uint n_normal_bits = 3u;
    uint normal_mask = (1u << n_normal_bits) - 1u;
    uint light_mask = 255u;

    uint x = pos_mask & (pack >> (0u * n_bits));
    uint y = pos_mask & (pack >> (1u * n_bits));
    uint z = pos_mask & (pack >> (2u * n_bits));
    uint offset_bits = offset_mask & (pack >> (3u * n_bits));
    uint normal_index = normal_mask & (pack >> (3u * n_bits + n_offset_bits));
    uint light_bits = light_mask & (pack >> (3u * n_bits + n_offset_bits + n_normal_bits));

    position = vec3(
            float(x) + 0.5 - float(1u & (offset_bits >> 2u)),
            float(y) + 0.5 - float(1u & (offset_bits >> 1u)),
            float(z) + 0.5 - float(1u & (offset_bits >> 0u))
        );

    normal = normals[normal_index];
    tangent = tangents[normal_index];
    light = float(light_bits) / 255.0;
}

void main() {
    vec3 position;
    vec3 v_Tangent;
    float v_light;
    unpack_data(floatBitsToUint(position_pack), position, norm, v_Tangent, v_light);

    vec4 worldPos = model * vec4(position, 1.0f);
    FragPos = worldPos.xyz;
    FragPosLightSpace = toLightSpace * worldPos;
    a_light = v_light;
    a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
    gl_Position = proj * view * worldPos;

    a_Tangent = normalize(v_Tangent);
    a_Bitangent = normalize(cross(norm, a_Tangent));
    toTangentSpace = mat3(
            a_Tangent.x, a_Bitangent.x, norm.x,
            a_Tangent.y, a_Bitangent.y, norm.y,
            a_Tangent.z, a_Bitangent.z, norm.z
        );
    toCam = /* toTangentSpace * */ ((inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - worldPos.xyz);
    v_toLightVec = /* toTangentSpace *  */ toLightVec;
}
