#version 330 core

in vec4 vColor;
in vec2 a_TexCoord;
in vec3 norm;
in vec3 toCam;
in vec3 a_Tangent;
in vec3 a_Bitangent;
in mat3 toTangentSpace;
in vec3 v_toLightVec;
out vec3 FragPos;
out vec4 FragPosLightSpace;

out vec4 color;

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform vec2 resolution;
uniform vec3 lightColor;

float shadowCalc() {
	vec3 pos = FragPosLightSpace.xyz * 0.5f + 0.5f;
	float depth = texture(u_Texture2, pos.xy).r;
	//return depth < pos.z ? 0.0f : 1.0f;
	return 1.0f;
}

void main() {
	//vec4 normColor = texture(u_Texture1, a_TexCoord);
	//float reflectivity = normColor.a * 0.5f;
	vec3 surfNorm = norm;

	float shineDamper = 1000.0f;
	float reflectivity = 0.25f;

	vec4 texColor = vColor * texture(u_Texture0, a_TexCoord);

	vec3 unitN = normalize(surfNorm);
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

	//vec3 texColor = texture(u_Texture0, a_TexCoord).rgb;
	//vec3 normal = normalize(norm);
	//float reflectivity = 0.5f;
	//float shineDamper = 10.0f;
//
	///* Ambient */
	//vec3 ambient = 0.3f * texColor;
//
	///* Diffuse */
	//vec3 lightDir = -normalize(v_toLightVec);
	//float nDOTl = dot(lightDir, normal);
	//float diff = max(nDOTl, 0.0f);
	//vec3 diffuse = diff * lightColor;
//
	///* Specular */
	//vec3 reflectedLight = reflect(lightDir, normal);
	//float specularFactor = dot(reflectedLight, normalize(toCam));
	//float dampedFactor = pow(specularFactor, shineDamper);
	//vec3 specular = vec3(dampedFactor * lightColor * reflectivity);

	/* Calculate shadow */
	/* float shadow = shadowCalc();
	vec3 lighting = (shadow * (mixcolor + finalSpec)) * vec3(texColor); */

	if (gl_FragCoord.x >= resolution.x / 2 - 1 &&
		gl_FragCoord.x <= resolution.x / 2 + 1 &&
		gl_FragCoord.y >= resolution.y / 2 - 8 &&
		gl_FragCoord.y <= resolution.y / 2 + 8 ||
		gl_FragCoord.x >= resolution.x / 2 - 8 &&
		gl_FragCoord.x <= resolution.x / 2 + 8 &&
		gl_FragCoord.y >= resolution.y / 2 - 0.5 &&
		gl_FragCoord.y <= resolution.y / 2 + 1) {
		vec3 i = finalSpec.rgb + vec3((texColor.r + texColor.g + texColor.b) / 3.0f);
		color = vec4(vec3(1.0f - i) * 2.0f, 1.0f);
	} else {
		color = vColor * texture(u_Texture0, a_TexCoord);//vec4(finalSpec.rgb + mixcolor, 1.0f);
	}
}
