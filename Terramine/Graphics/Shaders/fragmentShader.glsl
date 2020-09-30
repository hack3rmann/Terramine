#version 330 core

in vec4 vColor;
in vec2 a_TexCoord;
in vec3 surfNorm;
in vec3 toCam;

out vec4 color;

uniform sampler2D u_Texture0;
uniform vec2 resolution;
uniform vec3 lightColor;
uniform vec3 toLightVec;

void main() {
	float shineDamper = 10.0f;
	float reflectivity = 0.6f;

	vec4 texColor = vColor * texture(u_Texture0, a_TexCoord);

	vec3 unitN = normalize(surfNorm);
	vec3 unitL = normalize(toLightVec);
	vec3 lightDir = -unitL;
	vec3 unitC = normalize(toCam);

	vec3 reflectedLight = reflect(lightDir, unitN);

	float specularFactor = dot(reflectedLight, unitC);
	specularFactor = max(specularFactor, 0.0f);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec4 finalSpec = vec4(dampedFactor * lightColor * reflectivity, 0.2f);

	float nDOTl = dot(unitN, unitL);
	float brightness = max(nDOTl, 0.5f);
	vec4 diffuse = vec4(brightness * lightColor, 1.0f);

	vec4 mixcolor = mix(texColor, diffuse, brightness);

	if (gl_FragCoord.x >= resolution.x / 2 - 1 &&
		gl_FragCoord.x <= resolution.x / 2 + 1 &&
		gl_FragCoord.y >= resolution.y / 2 - 8 &&
		gl_FragCoord.y <= resolution.y / 2 + 8 ||
		gl_FragCoord.x >= resolution.x / 2 - 8 &&
		gl_FragCoord.x <= resolution.x / 2 + 8 &&
		gl_FragCoord.y >= resolution.y / 2 - 0.5 &&
		gl_FragCoord.y <= resolution.y / 2 + 1) {
		vec3 i = finalSpec.rgb + vec3((texColor.r + texColor.g + texColor.b) / 3);
		color = /* mixcolor *  */vec4(vec3(1.0f - i) * 2.0f, 1.0f);
	} else {
		color = finalSpec + mixcolor * texColor;
	}
}
