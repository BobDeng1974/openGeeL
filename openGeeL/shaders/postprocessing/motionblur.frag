#version 430 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D image;

const unsigned int maxSamples = 20;
uniform float sampleSize = 5.f;

uniform vec3 offsets[maxSamples];
uniform float kernel[maxSamples];


void main() {
	vec3 result = vec3(0.f);
	for(unsigned int i = 0; i < int(sampleSize); i++)
		result += texture(image, TexCoords - offsets[i].xy).rgb * kernel[i]; 

	color = vec4(result, 1.f);
}