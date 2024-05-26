#version 330 core

in vec2 a_TexCoord;
out vec4 color;

uniform sampler2D screenColor;
uniform int hue;

float m = 1.0f / 1080.0f;
/* float kerenl[] = float[] (
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
	2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
	1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
); */
/* float kerenl[] = float[] (
	1.0f,  1.0f, 1.0f,
	1.0f,  8.0f, 1.0f,
	1.0f,  1.0f, 1.0f
); */
float kerenl[] = float[] (
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);

vec3 RGBtoHSL(vec3 inputColor) {
	float Cmax = max(max(inputColor.r, inputColor.g), inputColor.b);
	float Cmin = min(min(inputColor.r, inputColor.g), inputColor.b);
	float delta = Cmax - Cmin;

	float h, s, l;

	l = (Cmin + Cmax) / 2.0f;

	if (delta == 0.0f) {
		h = 0.0f;
		s = 0.0f;
	} else {
		s = delta / (1 - abs(2.0f * l - 1.0f));
	}
	if (Cmax == inputColor.r)
		h = 60.0f * mod(((inputColor.g - inputColor.b) / delta), 6);
	if (Cmax == inputColor.g)
		h = 60.0f * (((inputColor.b - inputColor.r) / delta) + 2.0f);
	if (Cmax == inputColor.b)
		h = 60.0f * (((inputColor.r - inputColor.g) / delta) + 4.0f);

	return vec3(h, s, l);
}
vec3 HSLtoRGB(vec3 inputColor) { // r->h, g->s, b->l
	float c = inputColor.g * (1.0f - abs(2.0f * inputColor.b - 1.0f));
	float x = c * (1 - abs(mod(inputColor.r / 60.0, 2.0f) - 1.0f));
	float m = inputColor.b - c / 2.0f;
	float h = inputColor.r;

	vec3 res;

	if (h >= 0.0f && h < 60.0f)
		res = vec3(c, x, 0);
	if (h >= 60.0f && h < 120.0f)
		res = vec3(x, c, 0);
	if (h >= 120.0f && h < 180.0f)
		res = vec3(0, c, x);
	if (h >= 180.0f && h < 240.0f)
		res = vec3(0, x, c);
	if (h >= 240.0f && h < 300.0f)
		res = vec3(x, 0, c);
	if (h >= 300.0f && h < 360.0f)
		res = vec3(c, 0, c);

	res = vec3(res.r + m, res.g + m, res.b + m);

	return res;
}

void main() {
/* 	vec4 col = vec4(0.0f);
	col += texture(screenColor, a_TexCoord + vec2(-m,  m)) * kerenl[0];
	col += texture(screenColor, a_TexCoord + vec2( 0,  m)) * kerenl[1];
	col += texture(screenColor, a_TexCoord + vec2( m,  m)) * kerenl[2];
	col += texture(screenColor, a_TexCoord + vec2(-m,  0)) * kerenl[3];
	col += texture(screenColor, a_TexCoord + vec2( 0,  0)) * kerenl[4];
	col += texture(screenColor, a_TexCoord + vec2( m,  0)) * kerenl[5];
	col += texture(screenColor, a_TexCoord + vec2(-m, -m)) * kerenl[6];
	col += texture(screenColor, a_TexCoord + vec2( 0, -m)) * kerenl[7];
	col += texture(screenColor, a_TexCoord + vec2( m, -m)) * kerenl[8];
	///float brightness = (col.r + col.b + col.a) / 3.0;
	color = col; */

	vec4 inputColor = texture(screenColor, a_TexCoord);
	vec3 hslCol = RGBtoHSL(inputColor.rgb);
	//hslCol.g += hue / 360.0f;
	//hslCol.r += hue;

	color = vec4(HSLtoRGB(hslCol), 1.0f);
}