#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D prevFrame;

uniform float maxSamples = 5.f;
uniform float strength;
uniform vec3 offset;

void main() {
	vec3 current = texture(image, TexCoords).rgb; 

	vec3 previous = vec3(0.f);
	for(float i = 0; i < maxSamples; i += 1.f) {
		vec2 off = (i / maxSamples) * offset.xy;
		previous += texture(prevFrame, TexCoords + off).rgb; 
	}

	previous /= maxSamples;

	vec3 result = (1.f - strength) * current + strength * previous;
	color = vec4(result, 1.f);
}