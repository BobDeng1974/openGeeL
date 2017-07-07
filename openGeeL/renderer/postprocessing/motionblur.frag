#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;
uniform sampler2D prevFrame;

uniform float strength;
uniform vec3 offset;

void main() {
	vec3 current = texture(image, TexCoords).rgb; 

	vec3 previous = vec3(0.f);
	float maxi = 5.f;
	for(float i = 0; i < maxi; i += 1.f) {
		vec2 off = (i / maxi) * offset.xy;

		previous += texture(prevFrame, TexCoords + off).rgb; 
	}

	previous /= maxi;

	vec3 result = (1.f - strength) * current + strength * previous;
	color = vec4(result, 1.f);
}