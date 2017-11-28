#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

uniform float maxSamples = 5.f;
uniform float strength;
uniform vec3 offset;

void main() {

	vec3 result = vec3(0.f);
	for(float i = 0; i < maxSamples; i += 1.f) {
		vec2 off = (i / maxSamples) * offset.xy * strength;
		result += texture(image, TexCoords - off).rgb; 
	}

	color = vec4(result / maxSamples, 1.f);
}