#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D velocity;

uniform float sampleSize = 5.f;
uniform float strength;

void main() {
	vec3 offset = texture(velocity, TexCoords).xyz;

	vec3 result = vec3(0.f);
	for(float i = 0; i < sampleSize; i += 1.f) {
		vec2 off = (i / sampleSize) * offset.xy * strength;
		result += texture(image, TexCoords + off).rgb; 
	}

	color = vec4(result / sampleSize, 1.f);
}