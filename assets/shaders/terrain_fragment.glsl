#version 330 core

in float a_light;
in vec2 a_TexCoord;
in vec3 norm;
in vec3 toCam;
in vec3 v_toLightVec;

out vec4 color;

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform vec2 resolution;
uniform vec3 lightColor;

void main() {
    vec3 surfNorm = norm;

    float shineDamper = 1000.0f;
    float reflectivity = 0.25f;

    vec4 texColor = texture(u_Texture0, a_TexCoord);

    vec3 unitN = surfNorm;
    vec3 unitL = normalize(v_toLightVec);
    vec3 lightDir = -unitL;
    vec3 unitC = normalize(toCam);

    vec3 reflectedLight = reflect(lightDir, unitN);

    float specularFactor = dot(reflectedLight, unitC);
    specularFactor = max(specularFactor, 0.5f);
    float dampedFactor = pow(specularFactor, shineDamper);
    vec3 finalSpec = vec3(dampedFactor * lightColor * reflectivity);

    float nDOTl = dot(unitN, unitL);
    float brightness = max(nDOTl, 0.1f);
    vec3 diffuse = vec3(brightness * lightColor);

    vec3 mixcolor = mix(diffuse, texColor.rgb, brightness);

    color = vec4(a_light * texColor.rgb, texColor.a);
}
