#version 330 core

layout(location = 0) in float position_pack;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in float v_light;
layout(location = 4) in vec3 v_Tangent;

out vec4 vColor;
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

vec3 unpack_position(uint pack) {
    uint n_bits = 4u;
    uint pos_mask = (1u << n_bits) - 1u;
    uint offset_mask = 7u;

    uint x = pos_mask & (pack >> (0u * n_bits));
    uint y = pos_mask & (pack >> (1u * n_bits));
    uint z = pos_mask & (pack >> (2u * n_bits));
    uint offset_bits = offset_mask & (pack >> (3u * n_bits));

    return vec3(
        float(x) + 0.5 - float(1u & (offset_bits >> 2u)),
        float(y) + 0.5 - float(1u & (offset_bits >> 1u)),
        float(z) + 0.5 - float(1u & (offset_bits >> 0u))
    );
}

void main() {
    vec3 position = unpack_position(floatBitsToUint(position_pack));

    vec4 worldPos = model * vec4(position, 1.0f);
    FragPos = worldPos.xyz;
    FragPosLightSpace = toLightSpace * worldPos;
    vColor = vec4(v_light, v_light, v_light, 1.0);
    a_TexCoord = vec2(v_TexCoord.x, 1 - v_TexCoord.y);
    gl_Position = proj * view * worldPos;

    norm = normalize(normal);
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
