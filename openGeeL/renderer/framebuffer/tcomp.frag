#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D accumulation;
uniform sampler2D revealage;

void main() { 
	vec4 accu = texture(accumulation, TexCoords);
	float rev = texture(revealage, TexCoords).r;

	vec3 avgColor = accu.rgb / max(accu.a, 1e-5);
	color = vec4(avgColor, 1.f - rev);
}