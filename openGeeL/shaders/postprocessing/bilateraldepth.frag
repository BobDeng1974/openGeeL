#version 430 core

#define POSITION_MAP gPositionDepth

in vec2 TexCoords;

out vec4 color;

const int kernelSize = 3;

uniform float weights[kernelSize];
uniform float offsets[kernelSize];

uniform sampler2D image;
uniform sampler2D POSITION_MAP;

#include <shaders/gbufferread.glsl>

uniform bool horizontal;
uniform float sigma;


float bilateralCoeffient(float baseDepth, float sampleDepth) {
	float diff = abs(baseDepth - sampleDepth);
	float fac = sigma;

	return exp(-(diff * diff) / (2.f * fac * fac));
}


void main() {             

	//Size of single texel
    vec2 texOffset = 1.f / textureSize(image, 0); 
    vec3 base = texture(image, TexCoords).rgb; 
	float baseDepth = readDepth(TexCoords);
	vec3 result = base * weights[0];

	float hor = step(1.f, float(horizontal));
	float ver = 1 - hor;

	float weightSum = weights[0];
	vec2 offset = texOffset * vec2(hor, ver);
	for(int i = 1; i < kernelSize; i++) {
		vec2 off = offset * offsets[i];

		vec3 samp = texture(image, TexCoords + off).rgb;
		float sampDepth = readDepth(TexCoords + off);
		float weight = bilateralCoeffient(baseDepth, sampDepth) * weights[i];
		weightSum += weight;

		//Sample right / top pixel
		result += samp * weight;

		samp = texture(image, TexCoords - off).rgb;
		sampDepth = readDepth(TexCoords - off);
		weight = bilateralCoeffient(baseDepth, sampDepth) * weights[i];
		weightSum += weight;

		//Sample left / bottom pixel
		result += samp * weight;
    }

    color = vec4(result / weightSum, 1.f);
}