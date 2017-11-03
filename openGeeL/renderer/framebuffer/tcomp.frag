#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D accumulation;
uniform sampler2D revealage;

void main() { 
	vec3 result = texture(accumulation, TexCoords).rgb;

	color = vec4(result, 1.f);
}